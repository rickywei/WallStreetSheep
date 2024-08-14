package daemon

type Tick struct {
	InstrumentId string
	TS           int64
	Open         float64
	Close        float64
	Highest      float64
	Lowest       float64
	Volume       float64
	Hold         float64
}

type Bar struct {
	InstrumentId string
	TS           int64
	Open         float64
	Close        float64
	Highest      float64
	Lowest       float64
	Volume       float64
	Hold         float64
}
