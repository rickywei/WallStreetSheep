package api

import (
	"fmt"
	"net/http"
	"time"

	"github.com/gin-gonic/gin"
	"github.com/samber/lo"
	"go.uber.org/zap"

	"github.com/rickywei/wallstreetsheep/logger"
	"github.com/rickywei/wallstreetsheep/model"
)

const (
	chCtpOrder = "chCtpOrder"
)

func ctpOrder(ctx *gin.Context) {
	data := &model.CtpOrder{}
	err := ctx.ShouldBindBodyWithJSON(&data.CtpOrderReq)
	if err != nil {
		logger.L().Error("bind failed", zap.Error(err))
		ctx.Status(http.StatusBadRequest)
		return
	}

	data.Date = lo.ToPtr(time.Now().Format(time.DateOnly))
	k := fmt.Sprintf("ctpRequestId:%s", *data.Date)
	requestId, err := model.RC.Incr(ctx, k).Result()
	defer model.RC.Expire(ctx, k, time.Hour*24)
	if err != nil {
		logger.L().Error("incr request id failed", zap.Error(err))
		ctx.Status(http.StatusInternalServerError)
		return
	}
	data.RequestId = lo.ToPtr(int(requestId))
	if err = model.WriteDb(ctx, data); err != nil {
		ctx.Status(http.StatusInternalServerError)
		return
	}
	if err = model.Publish(ctx, chCtpOrder, data); err != nil {
		ctx.Status(http.StatusInternalServerError)
		return
	}

	ctx.Status(http.StatusOK)
}
