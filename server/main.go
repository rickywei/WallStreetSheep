package main

import (
	"errors"
	"os"
	"os/signal"
	"syscall"

	"github.com/oklog/run"
	"go.uber.org/zap"

	_ "github.com/rickywei/wallstreetsheep/conf"
	"github.com/rickywei/wallstreetsheep/daemon"
	"github.com/rickywei/wallstreetsheep/logger"
)

func main() {
	rg := run.Group{}
	{
		term := make(chan os.Signal, 1)
		signal.Notify(term, os.Interrupt, syscall.SIGTERM)
		rg.Add(func() error {
			<-term
			return errors.New("terminated")
		}, func(err error) {})
	}
	{
		rg.Add(func() error {
			return daemon.RunCtp()
		}, func(err error) {
			daemon.StopCtp()
		})
	}
	if err := rg.Run(); err != nil {
		logger.L().Fatal("", zap.Error(err))
	}
}