package daemon

import (
	"context"
	"encoding/json"
	"errors"
	"fmt"
	"os"
	"time"

	"github.com/redis/go-redis/v9"
	"go.uber.org/zap"
	"gorm.io/driver/postgres"
	"gorm.io/gorm"

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
	rc          redis.UniversalClient
	db          *gorm.DB
)

func init() {
	var err error
	rc = redis.NewUniversalClient(&redis.UniversalOptions{
		Addrs: conf.Strings("redis.addr"),
	})
	if _, err = rc.Ping(context.Background()).Result(); err != nil {
		logger.L().Fatal("ping redis failed", zap.Error(err))
	}

	db, err = gorm.Open(postgres.New(postgres.Config{
		DSN: fmt.Sprintf("host=%s port=%d user=%s password=%s dbname=%s",
			conf.String("postgres.host"),
			conf.Int("postgres.port"),
			conf.String("postgres.user"),
			conf.String("postgres.password"),
			conf.String("postgres.dbname"),
		),
	}))
	if err != nil {
		logger.L().Fatal("connect postgres failed", zap.Error(err))
	}
}

func RunCtp() error {
	ps := rc.Subscribe(ctx, chCtpHeartbeat)
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
				writeDb(ctx, db, ct)
			case chCtpOnTrade:
			}
		case <-tk.C:
			logger.L().Error("heartbeat more than 3 seconds")
			pid.Kill()
		case <-ctx.Done():
			return errors.New("ctx done")
		}
	}
}

func StopCtp() {
	cancel()
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
