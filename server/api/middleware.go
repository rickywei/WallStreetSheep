package api

import (
	"net/http"
	"time"

	"github.com/gin-gonic/gin"
	"github.com/rickywei/wallstreetsheep/conf"
	"github.com/rickywei/wallstreetsheep/logger"
	"github.com/samber/lo"
	"go.uber.org/zap"
)

func ginLog() gin.HandlerFunc {
	return func(ctx *gin.Context) {
		start := time.Now()

		ctx.Next()

		logger.L().Info(ctx.Request.URL.String(),
			zap.String("method", ctx.Request.Method),
			zap.Int("status", ctx.Writer.Status()),
			zap.Duration("cost", time.Since(start)),
			zap.String("ip", ctx.ClientIP()),
		)
	}
}

func authTradingViewIp() gin.HandlerFunc {
	return func(ctx *gin.Context) {
		ip := ctx.ClientIP()
		if !lo.Contains(conf.Strings("ctp.tradingview.ips"), ip) {
			logger.L().Error("invalid trading view ip", zap.String("ip", ip))
			ctx.AbortWithStatus(http.StatusForbidden)
			return
		}
		ctx.Next()
	}
}
