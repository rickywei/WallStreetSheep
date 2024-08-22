package api

import (
	"fmt"
	"net/http"
	"time"

	"github.com/gin-gonic/gin"
	"github.com/samber/lo"
	"go.uber.org/zap"

	"github.com/rickywei/wallstreetsheep/daemon"
	"github.com/rickywei/wallstreetsheep/logger"
	"github.com/rickywei/wallstreetsheep/model"
)

const (
	fmtTradingDay = "20060102"
	chCtpOrder    = "chCtpOrder"
	chCtpCancel   = "chCtpCancel"
	kTradingDay   = "ctpTradingDay"
)

func ctpOrder(ctx *gin.Context) {
	data := &model.CtpOrder{}
	err := ctx.ShouldBindBodyWithJSON(&data.CtpOrderReq)
	if err != nil {
		logger.L().Error("bind failed", zap.Error(err))
		ctx.AbortWithStatus(http.StatusBadRequest)
		return
	}

	td, err := model.RC.Get(ctx, kTradingDay).Result()
	if err != nil {
		logger.L().Error("get trading day failed", zap.Error(err))
		ctx.AbortWithStatus(http.StatusInternalServerError)
		return
	}
	data.TradingDay = lo.ToPtr(td)
	k := fmt.Sprintf("ctpRequestId:%s", *data.TradingDay)
	requestId, err := model.RC.Incr(ctx, k).Result()
	defer model.RC.Expire(ctx, k, time.Hour*24)
	if err != nil {
		logger.L().Error("incr request id failed", zap.Error(err))
		ctx.AbortWithStatus(http.StatusInternalServerError)
		return
	}
	data.RequestId = lo.ToPtr(int(requestId))
	if err = model.WriteDb(ctx, data); err != nil {
		ctx.AbortWithStatus(http.StatusInternalServerError)
		return
	}
	daemon.SendFeishu("ctp.feishu.orderInsert", data)
	if err = model.Publish(ctx, chCtpOrder, data); err != nil {
		ctx.AbortWithStatus(http.StatusInternalServerError)
		return
	}

	ctx.Status(http.StatusOK)
}

func ctpCancel(ctx *gin.Context) {
	data := &model.CtpCancel{}
	err := ctx.ShouldBindBodyWithJSON(&data.CtpCancelReq)
	if err != nil {
		logger.L().Error("bind failed", zap.Error(err))
		ctx.AbortWithStatus(http.StatusBadRequest)
		return
	}
	if data.ExchangeId == nil || data.InstrumentId == nil || data.OrderSysId == nil {
		logger.L().Error("nil arguments", zap.Error(err))
		ctx.AbortWithStatus(http.StatusBadRequest)
		return
	}
	if err = model.WriteDb(ctx, data); err != nil {
		ctx.AbortWithStatus(http.StatusInternalServerError)
		return
	}
	if err = model.Publish(ctx, chCtpCancel, data); err != nil {
		ctx.AbortWithStatus(http.StatusInternalServerError)
		return
	}

	ctx.Status(http.StatusOK)
}
