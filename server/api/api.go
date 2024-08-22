package api

import (
	"context"
	"net/http"

	"github.com/gin-gonic/gin"
	"github.com/rickywei/wallstreetsheep/logger"
	"go.uber.org/zap"
)

var (
	ctx, cancel = context.WithCancel(context.Background())
	srv         = &http.Server{}
)

func RunApi() error {
	gin.SetMode(gin.DebugMode)
	r := gin.New()
	r.Use(ginLog(), gin.Recovery())
	r.SetTrustedProxies([]string{"0.0.0.0/0", "::/0"})
	ctp := r.Group("ctp").Use()
	{
		ctp.POST("order", ctpOrder)
		ctp.POST("cancel", ctpCancel)
	}

	srv.Addr = ":8888"
	srv.Handler = r
	err := srv.ListenAndServe()
	if err != nil {
		logger.L().Fatal("http listen failed", zap.Error(err))
	}
	return err
}

func StopApi() {
	defer cancel()
	srv.Shutdown(ctx)
}
