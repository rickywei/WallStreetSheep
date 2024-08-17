package model

import (
	"encoding/json"

	"go.uber.org/zap"

	"github.com/rickywei/wallstreetsheep/logger"
)

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

func NewCtpInstrument(str string) (c *CtpInstrument, err error) {
	c = &CtpInstrument{}
	err = json.Unmarshal([]byte(str), c)
	if err != nil {
		logger.L().Error("unmarshal ctp failed", zap.String("msg", str), zap.Error(err))
	}
	return
}
