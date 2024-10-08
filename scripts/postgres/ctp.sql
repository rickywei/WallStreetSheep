-- Active: 1722392652205@@127.0.0.1@5432@WallStreetSheep
CREATE DATABASE "WallStreetSheep";

CREATE TABLE "CtpMarket" (
    "TradingDay" TEXT,
    -- "reserve1" TEXT,
    -- "ExchangeID" TEXT,
    -- "reserve2" TEXT,
    "LastPrice" DOUBLE PRECISION,
    "PreSettlementPrice" DOUBLE PRECISION,
    "PreClosePrice" DOUBLE PRECISION,
    "PreOpenInterest" DOUBLE PRECISION,
    "OpenPrice" DOUBLE PRECISION,
    "HighestPrice" DOUBLE PRECISION,
    "LowestPrice" DOUBLE PRECISION,
    "Volume" DOUBLE PRECISION,
    "Turnover" DOUBLE PRECISION,
    "OpenInterest" DOUBLE PRECISION,
    "ClosePrice" DOUBLE PRECISION,
    "SettlementPrice" DOUBLE PRECISION,
    "UpperLimitPrice" DOUBLE PRECISION,
    "LowerLimitPrice" DOUBLE PRECISION,
    "PreDelta" DOUBLE PRECISION,
    "CurrDelta" DOUBLE PRECISION,
    "UpdateTime" TEXT,
    "UpdateMillisec" INT,
    "BidPrice1" DOUBLE PRECISION,
    "BidVolume1" DOUBLE PRECISION,
    "AskPrice1" DOUBLE PRECISION,
    "AskVolume1" DOUBLE PRECISION,
    -- "BidPrice2" DOUBLE PRECISION,
    -- "BidVolume2" DOUBLE PRECISION,
    -- "AskPrice2" DOUBLE PRECISION,
    -- "AskVolume2" DOUBLE PRECISION,
    -- "BidPrice3" DOUBLE PRECISION,
    -- "BidVolume3" DOUBLE PRECISION,
    -- "AskPrice3" DOUBLE PRECISION,
    -- "AskVolume3" DOUBLE PRECISION,
    -- "BidPrice4" DOUBLE PRECISION,
    -- "BidVolume4" DOUBLE PRECISION,
    -- "AskPrice4" DOUBLE PRECISION,
    -- "AskVolume4" DOUBLE PRECISION,
    -- "BidPrice5" DOUBLE PRECISION,
    -- "BidVolume5" DOUBLE PRECISION,
    -- "AskPrice5" DOUBLE PRECISION,
    -- "AskVolume5" DOUBLE PRECISION,
    "AveragePrice" DOUBLE PRECISION,
    "ActionDay" TEXT,
    "InstrumentID" TEXT,
    -- "ExchangeInstID" TEXT,
    -- "BandingUpperPrice" DOUBLE PRECISION,
    -- "BandingLowerPrice" DOUBLE PRECISION,
    "TS" BIGINT,
    PRIMARY KEY (
        "InstrumentID",
        "UpdateTime",
        "UpdateMillisec"
    )
);

CREATE TABLE "CtpInstrument" (
    "CreatedAt" BIGINT,
    "UpdatedAt" BIGINT,
    -- "reserve1" TEXT,
    "ExchangeID" TEXT,
    "InstrumentName" TEXT,
    -- "reserve2" TEXT,
    -- "reserve3" TEXT,
    "ProductClass" TEXT,
    "DeliveryYear" INT,
    "DeliveryMonth" INT,
    "MaxMarketOrderVolume" INT,
    "MinMarketOrderVolume" INT,
    "MaxLimitOrderVolume" INT,
    "MinLimitOrderVolume" INT,
    "VolumeMultiple" INT,
    "PriceTick" DOUBLE PRECISION,
    "CreateDate" TEXT,
    "OpenDate" TEXT,
    "ExpireDate" TEXT,
    "StartDelivDate" TEXT,
    "EndDelivDate" TEXT,
    "InstLifePhase" TEXT,
    "IsTrading" INT,
    "PositionType" TEXT,
    "PositionDateType" TEXT,
    "LongMarginRatio" DOUBLE PRECISION,
    "ShortMarginRatio" DOUBLE PRECISION,
    "MaxMarginSideAlgorithm" TEXT,
    -- "reserve4" TEXT,
    "StrikePrice" DOUBLE PRECISION,
    "OptionsType" TEXT,
    "UnderlyingMultiple" DOUBLE PRECISION,
    "CombinationType" TEXT,
    "InstrumentID" TEXT,
    "ExchangeInstID" TEXT,
    "ProductID" TEXT,
    "UnderlyingInstrID" TEXT,
    PRIMARY KEY ("InstrumentID")
);

CREATE TABLE "CtpOrder" (
    "CreatedAt" BIGINT,
    "UpdatedAt" BIGINT,
    "ExchangeId" TEXT,
    "InstrumentId" TEXT,
    "Direction" TEXT,
    "Offset" TEXT,
    "Price" DOUBLE PRECISION,
    "Volume" INT,
    "Mode" TEXT,
    "TradingDay" TEXT,
    "RequestId" INT,
    "OrderSysId" TEXT,
    "Status" TEXT,
    "Error" TEXT,
    PRIMARY KEY ("TradingDay", "RequestId")
);

CREATE TABLE "CtpCancel" (
    "CreatedAt" BIGINT,
    "UpdatedAt" BIGINT,
    "ExchangeId" TEXT,
    "InstrumentId" TEXT,
    "OrderSysId" TEXT,
    "Error" TEXT,
    PRIMARY KEY ("ExchangeId", "OrderSysId")
)

CREATE TABLE "CtpTrade" (
    "CreatedAt" BIGINT,
    "UpdatedAt" BIGINT,
    "TradingDay" TEXT,
    "ExchangeId" TEXT,
    "InstrumentId" TEXT,
    "OrderSysId" TEXT,
    "TradeId" TEXT,
    "Price" DOUBLE PRECISION,
    "Volume" INT,
    "Error" TEXT
);