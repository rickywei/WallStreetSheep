package model

type CtpInstrument struct {
	CreatedAt              int64    `gorm:"column:CreatedAt"`
	UpdatedAt              int64    `gorm:"column:UpdatedAt"`
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

type CtpOrderReq struct {
	ExchangeId   *string `json:"ExchangeId" gorm:"column:ExchangeId"`
	InstrumentId *string `json:"InstrumentId" gorm:"column:InstrumentId"`
	// '0' buy '1' sell
	Direction *string `json:"Direction" gorm:"column:Direction"`
	// '0' open '1' close
	Offset *string  `json:"Offset" gorm:"column:Offset"`
	Price  *float64 `json:"Price" gorm:"column:Price"`
	Volume *int     `json:"Volume" gorm:"column:Volume"`
	// FAK FOK
	Mode *string `json:"Mode" gorm:"column:Mode"`
}

type CtpOrder struct {
	*CtpOrderReq
	CreatedAt  int64   `json:"CreatedAt" gorm:"column:CreatedAt"`
	UpdatedAt  int64   `json:"UpdatedAt" gorm:"column:UpdatedAt"`
	Date       *string `json:"Date" gorm:"column:Date"`
	RequestId  *int    `json:"RequestId" gorm:"column:RequestId"`
	OrderSysId *string `json:"OrderSysId" gorm:"column:OrderSysId"`
	Error      *string `json:"Error" gorm:"column:Error"`
}

func (c *CtpOrder) TableName() string {
	return "CtpOrder"
}

type CtpTrade struct {
	CreatedAt    int64    `gorm:"column:CreatedAt"`
	UpdatedAt    int64    `gorm:"column:UpdatedAt"`
	ExchangeId   *string  `gorm:"column:ExchangeId"`
	InstrumentId *string  `gorm:"column:InstrumentId"`
	OrderSysId   *string  `gorm:"column:OrderSysId"`
	TradeId      *string  `gorm:"column:TradeId"`
	Price        *float64 `gorm:"column:Price"`
	Volume       *int     `gorm:"column:Volume"`
	Error        *string  `gorm:"column:Error"`
}

func (c *CtpTrade) TableName() string {
	return "CtpTrade"
}
