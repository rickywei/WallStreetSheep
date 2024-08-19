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
	chCtpHeartbeat        = "chCtpHeartbeat"
	chCtpInstrument       = "chCtpInstrument"
	chCtpOnRspOrderInsert = "chCtpOnRspOrderInsert"
	chCtpOnErrOrderInsert = "chCtpOnErrOrderInsert"
	chCtpOnRtnOrder       = "chCtpOnRtnOrder"
	chCtpOnRtnTrade       = "chCtpOnRtnTrade"
	kI2E                  = "ctpI2E"
)

var (
	ctx, cancel = context.WithCancel(context.Background())
	pid         *os.Process
)

func RunCtp() error {
	var err error
	ps := model.RC.Subscribe(ctx, chCtpHeartbeat, chCtpOnRspOrderInsert, chCtpOnErrOrderInsert, chCtpOnRtnOrder, chCtpOnRtnTrade)
	defer ps.Close()
	ch := ps.Channel()
	d := time.Second * 3
	tk := time.NewTicker(d)
	pid, err = os.StartProcess(conf.String("ctp.bin"), []string{marshalCtpConf()}, &os.ProcAttr{
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
				data, err := model.NewFromJson[model.CtpInstrument](msg.Payload)
				if err != nil {
					continue
				}
				model.WriteDb(ctx, data)
			case chCtpOnRspOrderInsert:
				data, err := model.NewFromJson[model.CtpOrder](msg.Payload)
				if err != nil {
					continue
				}
				if err = model.DB.Where(`"Date"=? AND "RequestId"=?`, data.Date, data.RequestId).Select("Error").Updates(data).Error; err != nil {
					logger.L().Error("chCtpOnRspOrderInsert failed", zap.Error(err))
				}
			case chCtpOnRtnOrder:
				data, err := model.NewFromJson[model.CtpOrder](msg.Payload)
				if err != nil {
					continue
				}
				if err = model.DB.Where(`"Date"=? AND "RequestId"=?`, data.Date, data.RequestId).Select("Status", "OrderSysID").Updates(data).Error; err != nil {
					logger.L().Error("chCtpOnRtnOrder failed", zap.Error(err))
				}
			case chCtpOnRtnTrade:
				data, err := model.NewFromJson[model.CtpTrade](msg.Payload)
				if err != nil {
					continue
				}
				model.WriteDb(ctx, data)
			}
		case <-tk.C:
			logger.L().Error("heartbeat more than 3 seconds")
			continue
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
	defer pid.Kill()
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
