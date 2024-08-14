package daemon

import (
	"context"
	"encoding/json"
	"errors"
	"fmt"
	"strings"
	"time"

	"github.com/redis/go-redis/v9"
	"github.com/samber/lo"
	"go.uber.org/zap"
	"gorm.io/driver/postgres"
	"gorm.io/gorm"

	"github.com/rickywei/wallstreetsheep/conf"
	"github.com/rickywei/wallstreetsheep/logger"
)

const (
	chMarket     = "ctpMarket"
	chInstrument = "ctpInstrument"
	chFmtTick    = "ctpTick:%s"
	chSubs       = "ctpSubs"
	chFmtSubs    = "ctpSubs:%s"
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
	ps := rc.Subscribe(ctx, chMarket, chInstrument)
	defer ps.Close()
	ch := ps.Channel()
	tk := time.NewTicker(time.Hour)
	for {
		select {
		case msg := <-ch:
			switch msg.Channel {
			case chMarket:
				cm, err := NewCtpMarket(msg.Payload)
				if err != nil {
					continue
				}
				writeDb(ctx, db, cm)
				if t := cm.ToTick(); t != nil {
					publish(ctx, rc, fmt.Sprintf(chFmtTick, t.InstrumentId), t)
				}
			case chInstrument:
				ct, err := NewCtpMarket(msg.Payload)
				if err != nil {
					continue
				}
				writeDb(ctx, db, ct)
				if strings.Contains(*ct.InstrumentID, "rb") {
					fmt.Println(*ct.InstrumentID)
					publish(ctx, rc, chSubs, ct.InstrumentID)
					k := fmt.Sprintf(chFmtSubs, time.Now().Format(time.DateOnly))
					if _, err := rc.SAdd(ctx, k, *ct.InstrumentID).Result(); err != nil {
						logger.L().Fatal("sadd failed", zap.String("instrumentId", *ct.InstrumentID), zap.Error(err))
						continue
					}
					rc.Expire(ctx, k, time.Hour*24)
				}
			}
		case <-tk.C:
			k := fmt.Sprintf(chFmtSubs, time.Now().Format(time.DateOnly))
			subs, err := rc.SMembers(ctx, k).Result()
			if err != nil {
				logger.L().Fatal("smembers failed", zap.Error(err))
				continue
			}
			lo.ForEach(subs, func(sub string, _ int) {
				publish(ctx, rc, chSubs, &sub)
			})
		case <-ctx.Done():
			return errors.New("ctx done")
		}
	}
}

func StopCtp() {
	cancel()
}

type CtpTick struct {
	TradingDay *string `gorm:"column:TradingDay"`
	// ExchangeID         *string  `gorm:"column:ExchangeID"`
	LastPrice          *float64 `gorm:"column:LastPrice"`
	PreSettlementPrice *float64 `gorm:"column:PreSettlementPrice"`
	PreClosePrice      *float64 `gorm:"column:PreClosePrice"`
	PreOpenInterest    *float64 `gorm:"column:PreOpenInterest"`
	OpenPrice          *float64 `gorm:"column:OpenPrice"`
	HighestPrice       *float64 `gorm:"column:HighestPrice"`
	LowestPrice        *float64 `gorm:"column:LowestPrice"`
	Volume             *float64 `gorm:"column:Volume"`
	Turnover           *float64 `gorm:"column:Turnover"`
	OpenInterest       *float64 `gorm:"column:OpenInterest"`
	ClosePrice         *float64 `gorm:"column:ClosePrice"`
	SettlementPrice    *float64 `gorm:"column:SettlementPrice"`
	UpperLimitPrice    *float64 `gorm:"column:UpperLimitPrice"`
	LowerLimitPrice    *float64 `gorm:"column:LowerLimitPrice"`
	PreDelta           *float64 `gorm:"column:PreDelta"`
	CurrDelta          *float64 `gorm:"column:CurrDelta"`
	UpdateTime         *string  `gorm:"column:UpdateTime"`
	UpdateMillisec     *int     `gorm:"column:UpdateMillisec"`
	BidPrice1          *float64 `gorm:"column:BidPrice1"`
	BidVolume1         *float64 `gorm:"column:BidVolume1"`
	AskPrice1          *float64 `gorm:"column:AskPrice1"`
	AskVolume1         *float64 `gorm:"column:AskVolume1"`
	// BidPrice2          *float64 `gorm:"column:BidPrice2"`
	// BidVolume2         *float64 `gorm:"column:BidVolume2"`
	// AskPrice2          *float64 `gorm:"column:AskPrice2"`
	// AskVolume2         *float64 `gorm:"column:AskVolume2"`
	// BidPrice3          *float64 `gorm:"column:BidPrice3"`
	// BidVolume3         *float64 `gorm:"column:BidVolume3"`
	// AskPrice3          *float64 `gorm:"column:AskPrice3"`
	// AskVolume3         *float64 `gorm:"column:AskVolume3"`
	// BidPrice4          *float64 `gorm:"column:BidPrice4"`
	// BidVolume4         *float64 `gorm:"column:BidVolume4"`
	// AskPrice4          *float64 `gorm:"column:AskPrice4"`
	// AskVolume4         *float64 `gorm:"column:AskVolume4"`
	// BidPrice5          *float64 `gorm:"column:BidPrice5"`
	// BidVolume5         *float64 `gorm:"column:BidVolume5"`
	// AskPrice5          *float64 `gorm:"column:AskPrice5"`
	// AskVolume5         *float64 `gorm:"column:AskVolume5"`
	AveragePrice *float64 `gorm:"column:AveragePrice"`
	ActionDay    *string  `gorm:"column:ActionDay"`
	InstrumentID *string  `gorm:"column:InstrumentID"`
	// ExchangeInstID     *string  `gorm:"column:ExchangeInstID"`
	// BandingUpperPrice  *float64 `gorm:"column:BandingUpperPrice"`
	// BandingLowerPrice  *float64 `gorm:"column:BandingLowerPrice"`
	TS *int64 `gorm:"column:TS"`
}

func (c *CtpTick) TableName() string {
	return "CtpTick"
}

func (c *CtpTick) ToTick() *Tick {
	defer func() {
		recover()
	}()
	return &Tick{
		InstrumentId: *c.InstrumentID,
		TS:           *c.TS,
		Open:         *c.OpenPrice,
		Close:        *c.ClosePrice,
		Highest:      *c.HighestPrice,
		Lowest:       *c.LowestPrice,
		Volume:       *c.Volume,
		Hold:         *c.OpenInterest,
	}
}

func NewCtpMarket(str string) (c *CtpTick, err error) {
	c = &CtpTick{}
	err = json.Unmarshal([]byte(str), c)
	if err != nil {
		logger.L().Error("unmarshal ctp failed", zap.String("msg", str), zap.Error(err))
	}
	tm, _ := time.Parse("20060102 15:04:05.000 -0700", fmt.Sprintf("%s %s.%03d +0800", *c.TradingDay, *c.UpdateTime, *c.UpdateMillisec))
	c.TS = lo.ToPtr(tm.UnixMilli())
	return
}

type CtpInstrument struct {
	ExchangeID             *string  `gorm:"column:ExchangeID"`
	InstrumentName         *string  `gorm:"column:InstrumentName"`
	ProductClass           *string  `gorm:"column:ProductClass"`
	DeliveryYear           *int     `gorm:"column:DeliveryYear"`
	DeliveryMonth          *int     `gorm:"column:DeliveryMonth"`
	MaxMarketOrderVolume   *int     `gorm:"column:MaxMarketOrderVolume"`
	MinMarketOrderVolume   *int     `gorm:"column:MinMarketOrderVolume"`
	MaxLimitOrderVolume    *int     `gorm:"column:MaxLimitOrderVolume"`
	MinLimitOrderVolume    *int     `gorm:"column:MinLimitOrderVolume"`
	VolumeMultiple         *int     `gorm:"column:VolumeMultiple"`
	PriceTick              *float64 `gorm:"column:PriceTick"`
	CreateDate             *string  `gorm:"column:CreateDate"`
	OpenDate               *string  `gorm:"column:OpenDate"`
	ExpireDate             *string  `gorm:"column:ExpireDate"`
	StartDelivDate         *string  `gorm:"column:StartDelivDate"`
	EndDelivDate           *string  `gorm:"column:EndDelivDate"`
	InstLifePhase          *string  `gorm:"column:InstLifePhase"`
	IsTrading              *int     `gorm:"column:IsTrading"`
	PositionType           *string  `gorm:"column:PositionType"`
	PositionDateType       *string  `gorm:"column:PositionDateType"`
	LongMarginRatio        *float64 `gorm:"column:LongMarginRatio"`
	ShortMarginRatio       *float64 `gorm:"column:ShortMarginRatio"`
	MaxMarginSideAlgorithm *string  `gorm:"column:MaxMarginSideAlgorithm"`
	StrikePrice            *float64 `gorm:"column:StrikePrice"`
	OptionsType            *string  `gorm:"column:OptionsType"`
	UnderlyingMultiple     *float64 `gorm:"column:UnderlyingMultiple"`
	CombinationType        *string  `gorm:"column:CombinationType"`
	InstrumentID           *string  `gorm:"column:InstrumentID"`
	ExchangeInstID         *string  `gorm:"column:ExchangeInstID"`
	ProductID              *string  `gorm:"column:ProductID"`
	UnderlyingInstrID      *string  `gorm:"column:UnderlyingInstrID"`
}

func (c *CtpInstrument) TableName() string {
	return "CtpInstrument"
}

func (c *CtpInstrument) Unmarshal(str string) (err error) {
	err = json.Unmarshal([]byte(str), c)
	if err != nil {
		logger.L().Error("unmarshal ctp failed", zap.String("msg", str), zap.Error(err))
	}
	return
}
