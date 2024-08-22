package daemon

import (
	"context"
	"encoding/json"
	"errors"
	"os"
	"sync"
	"time"

	"go.uber.org/zap"

	"github.com/redis/go-redis/v9"
	"github.com/rickywei/wallstreetsheep/conf"
	"github.com/rickywei/wallstreetsheep/logger"
	"github.com/rickywei/wallstreetsheep/model"
)

const (
	chCtpHeartbeat        = "chCtpHeartbeat"
	chCtpMarket           = "chCtpMarket"
	chCtpInstrument       = "chCtpInstrument"
	chCtpOnRspOrderInsert = "chCtpOnRspOrderInsert"
	chCtpOnErrOrderInsert = "chCtpOnErrOrderInsert"
	chCtpOnRtnOrder       = "chCtpOnRtnOrder"
	chCtpOnRtnTrade       = "chCtpOnRtnTrade"
	chCtpOnRspOrderAction = "chCtpOnRspOrderAction"
	chCtpOnErrOrderAction = "chCtpOnErrOrderAction"
	chCtpSubs             = "chCtpSubs"
	chCtpUnsubs           = "chCtpUnsubs"
)

var (
	ctx, cancel = context.WithCancel(context.Background())
	pid         *os.Process
	ps          *redis.PubSub
	ch          <-chan *redis.Message
	tk          = time.NewTicker(time.Second * 3)
	ch2func     = map[string]func(*redis.Message){
		chCtpHeartbeat:        chCtpHeartbeatHandler,
		chCtpMarket:           chCtpMarketHandler,
		chCtpInstrument:       chCtpInstrumentHandler,
		chCtpOnRspOrderInsert: chCtpOnRspErrOrderInsertHandler,
		chCtpOnErrOrderInsert: chCtpOnRspErrOrderInsertHandler,
		chCtpOnRtnOrder:       chCtpOnRtnOrderHandler,
		chCtpOnRtnTrade:       chCtpOnRtnTradeHandler,
		chCtpOnRspOrderAction: chCtpOnRspErrOrderActionHandler,
		chCtpOnErrOrderAction: chCtpOnRspErrOrderActionHandler,
	}
	heartbeat = sync.Map{}
)

func init() {
	var err error
	if pid, err = os.StartProcess(conf.String("ctp.bin"), []string{marshalCtpConf()}, &os.ProcAttr{
		Files: []*os.File{os.Stdin, os.Stdout, os.Stderr},
	}); err != nil {
		logger.L().Fatal("failed to start ctp", zap.Error(err))
	}
	ps = model.RC.Subscribe(ctx, chCtpHeartbeat, chCtpMarket,
		chCtpOnRspOrderInsert, chCtpOnErrOrderInsert, chCtpOnRtnOrder, chCtpOnRtnTrade)
	ch = ps.Channel()
}

func RunCtp() error {
	for {
		select {
		case msg := <-ch:
			f := ch2func[msg.Channel]
			if f == nil {
				logger.L().Error("cannot find handler", zap.String("channel", msg.Channel))
				continue
			}
			f(msg)
		case <-tk.C:
			restart()
		case <-ctx.Done():
			return errors.New("ctx done")
		}
	}
}

func StopCtp() {
	defer cancel()
	defer pid.Kill()
	defer ps.Close()
}

func marshalCtpConf() string {
	c := make(map[string]any)
	c["trade"] = conf.Bool("ctp.trade")
	c["market"] = conf.Bool("ctp.market")
	c["tradeFrontAddr"] = conf.String("ctp.tradeFrontAddr")
	c["marketFrontAddr"] = conf.String("ctp.marketFrontAddr")
	c["brokerId"] = conf.String("ctp.brokerId")
	c["investorId"] = conf.String("ctp.investorId")
	c["password"] = conf.String("ctp.password")
	c["flowPath"] = conf.String("ctp.flowPath")
	c["isUsingUdp"] = conf.Bool("ctp.isUsingUdp")
	c["isMulticast"] = conf.Bool("ctp.isMulticast")
	c["redisAddr"] = conf.String("ctp.redisAddr")
	bs, _ := json.Marshal(c)
	return string(bs)
}

func chCtpHeartbeatHandler(msg *redis.Message) {
	heartbeat.Store(msg.Payload, time.Now())
}

func chCtpMarketHandler(msg *redis.Message) {
	data, err := model.NewFromJson[model.CtpMarket](msg.Payload)
	if err != nil {
		return
	}
	model.WriteDb(ctx, data)
}

func chCtpInstrumentHandler(msg *redis.Message) {
	data, err := model.NewFromJson[model.CtpInstrument](msg.Payload)
	if err != nil {
		return
	}
	model.WriteDb(ctx, data)
}

func chCtpOnRspErrOrderInsertHandler(msg *redis.Message) {
	data, err := model.NewFromJson[model.CtpOrder](msg.Payload)
	if err != nil {
		return
	}
	db := model.DB.Where(`"TradingDay"=? AND "RequestId"=?`, data.TradingDay, data.RequestId).Select("Error").Updates(data)
	if db.Error != nil || db.RowsAffected <= 0 {
		logger.L().Error("chCtpOnRspOrderInsert failed", zap.String("msg", msg.Payload), zap.Int64("rowsAffected", db.RowsAffected), zap.Error(err))
	}
	SendFeishu("ctp.feishu.orderUpdate", data)
}

func chCtpOnRtnOrderHandler(msg *redis.Message) {
	data, err := model.NewFromJson[model.CtpOrder](msg.Payload)
	if err != nil {
		return
	}
	db := model.DB.Where(`"TradingDay"=? AND "RequestId"=?`, data.TradingDay, data.RequestId).Select("Status", "OrderSysID").Updates(data)
	if db.Error != nil || db.RowsAffected <= 0 {
		logger.L().Error("chCtpOnRtnOrder failed", zap.String("msg", msg.Payload), zap.Int64("rowsAffected", db.RowsAffected), zap.Error(err))
	}
	SendFeishu("ctp.feishu.orderUpdate", data)
}

func chCtpOnRtnTradeHandler(msg *redis.Message) {
	data, err := model.NewFromJson[model.CtpTrade](msg.Payload)
	if err != nil {
		return
	}
	model.WriteDb(ctx, data)
	SendFeishu("ctp.feishu.tradeInsert", data)
}

func chCtpOnRspErrOrderActionHandler(msg *redis.Message) {
	data, err := model.NewFromJson[model.CtpCancel](msg.Payload)
	if err != nil {
		return
	}
	db := model.DB.Where(`"ExchangeId"=? AND "OrderSysId"=?`, data.ExchangeId, data.OrderSysId).Select("Error").Updates(data)
	if db.Error != nil || db.RowsAffected <= 0 {
		logger.L().Error("chCtpOnRtnOrder failed", zap.String("msg", msg.Payload), zap.Int64("rowsAffected", db.RowsAffected), zap.Error(err))
	}
}

func restart() (err error) {
	tm, ok := heartbeat.Load("td")
	loss := conf.Bool("ctp.trade") && (!ok || time.Since(tm.(time.Time)) > time.Second*3)
	tm, ok = heartbeat.Load("md")
	loss = conf.Bool("ctp.market") && (!ok || time.Since(tm.(time.Time)) > time.Second*3)
	if !loss {
		return
	}

	logger.L().Error("heartbeat more than 3 seconds")
	pid.Kill()
	if pid, err = os.StartProcess(conf.String("ctp.bin"), []string{marshalCtpConf()}, &os.ProcAttr{
		Files: []*os.File{os.Stdin, os.Stdout, os.Stderr},
	}); err != nil {
		logger.L().Error("restart ctp failed", zap.Error(err))
	}
	return
}
