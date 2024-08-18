package api

import (
	"net/http"

	"github.com/gin-gonic/gin"
	"github.com/samber/lo"
	"go.uber.org/zap"

	"github.com/rickywei/wallstreetsheep/logger"
	"github.com/rickywei/wallstreetsheep/model"
)

const (
	chCtpOrder = "ctpOrder"
)

func ctpOrder(ctx *gin.Context) {
	bs, err := ctx.GetRawData()
	if err != nil {
		logger.L().Error("get order data failed", zap.Error(err))
		ctx.Status(http.StatusBadRequest)
	}
	logger.L().Info("received order", zap.String("data", string(bs)))
	model.Publish(ctx, chCtpOrder, lo.ToPtr(string(bs)))
	ctx.Status(http.StatusOK)
}
