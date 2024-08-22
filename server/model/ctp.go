package model

type CtpMarket struct {
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

func (c *CtpMarket) TableName() string {
	return "CtpMarket"
}

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
	TradingDay *string `json:"TradingDay" gorm:"column:TradingDay"`
	RequestId  *int    `json:"RequestId" gorm:"column:RequestId"`
	OrderSysId *string `json:"OrderSysId" gorm:"column:OrderSysId"`
	Status     *string `json:"Status" gorm:"column:Status"`
	Error      *string `json:"Error" gorm:"column:Error"`
}

func (c *CtpOrder) TableName() string {
	return "CtpOrder"
}

type CtpCancelReq struct {
	ExchangeId   *string `json:"ExchangeId" gorm:"column:ExchangeId"`
	InstrumentId *string `json:"InstrumentId" gorm:"column:InstrumentId"`
	OrderSysId   *string `json:"OrderSysId" gorm:"column:OrderSysId"`
}

type CtpCancel struct {
	*CtpCancelReq
	CreatedAt int64   `json:"CreatedAt" gorm:"column:CreatedAt"`
	UpdatedAt int64   `json:"UpdatedAt" gorm:"column:UpdatedAt"`
	Error     *string `json:"Error" gorm:"column:Error"`
}

type CtpTrade struct {
	CreatedAt    int64    `json:"CreatedAt" gorm:"column:CreatedAt"`
	UpdatedAt    int64    `json:"UpdatedAt" gorm:"column:UpdatedAt"`
	TradingDay   *string  `json:"TradingDay" gorm:"column:TradingDay"`
	ExchangeId   *string  `json:"ExchangeId" gorm:"column:ExchangeId"`
	InstrumentId *string  `json:"InstrumentId" gorm:"column:InstrumentId"`
	OrderSysId   *string  `json:"OrderSysId" gorm:"column:OrderSysId"`
	TradeId      *string  `json:"TradeId" gorm:"column:TradeId"`
	Price        *float64 `json:"Price" gorm:"column:Price"`
	Volume       *int     `json:"Volume" gorm:"column:Volume"`
	Error        *string  `json:"Error" gorm:"column:Error"`
}

func (c *CtpTrade) TableName() string {
	return "CtpTrade"
}
