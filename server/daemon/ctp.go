package daemon

import (
	"context"
	"encoding/json"
	"errors"
	"os"
	"time"

	"go.uber.org/zap"

	"github.com/rickywei/wallstreetsheep/conf"
	"github.com/rickywei/wallstreetsheep/logger"
	"github.com/rickywei/wallstreetsheep/model"
)

const (
	chCtpHeartbeat  = "ctpHeartbeat"
	chCtpInstrument = "ctpInstrument"
	chCtpOnTrade    = "ctpOnTrade"
)

var (
	ctx, cancel = context.WithCancel(context.Background())
)

func RunCtp() error {
	ps := model.RC.Subscribe(ctx, chCtpHeartbeat)
	defer ps.Close()
	ch := ps.Channel()
	d := time.Second * 3
	tk := time.NewTicker(d)
	pid, err := os.StartProcess(conf.String("ctp.bin"), []string{marshalCtpConf()}, &os.ProcAttr{
		Files: []*os.File{os.Stdin, os.Stdout, os.Stderr},
	})
	if err != nil {
		return err
	}
	for {
		select {
		case msg := <-ch:
			switch msg.Channel {
			case chCtpHeartbeat:
				tk.Reset(d)
			case chCtpInstrument:
				ct, err := model.NewCtpInstrument(msg.Payload)
				if err != nil {
					continue
				}
				model.WriteDb(ctx, ct)
			case chCtpOnTrade:
			}
		case <-tk.C:
			logger.L().Error("heartbeat more than 3 seconds")
			pid.Kill()
			if pid, err = os.StartProcess(conf.String("ctp.bin"), []string{marshalCtpConf()}, &os.ProcAttr{
				Files: []*os.File{os.Stdin, os.Stdout, os.Stderr},
			}); err != nil {
				logger.L().Error("restart ctp failed", zap.Error(err))
			}
		case <-ctx.Done():
			return errors.New("ctx done")
		}
	}
}

func StopCtp() {
	defer cancel()
}

func marshalCtpConf() string {
	c := make(map[string]any)
	c["frontAddr"] = conf.String("ctp.frontAddr")
	c["brokerId"] = conf.String("ctp.brokerId")
	c["investorId"] = conf.String("ctp.investorId")
	c["password"] = conf.String("ctp.password")
	c["redisAddr"] = conf.String("ctp.redisAddr")
	c["flowPath"] = conf.String("ctp.flowPath")
	bs, _ := json.Marshal(c)
	return string(bs)
}
