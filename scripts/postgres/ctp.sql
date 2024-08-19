-- Active: 1722392652205@@127.0.0.1@5432@WallStreetSheep
CREATE DATABASE "WallStreetSheep";

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
    "Date" TEXT,
    "RequestId" INT,
    "OrderSysId" TEXT,
    "Status" TEXT,
    "Error" TEXT,
    PRIMARY KEY ("Date", "RequestId")
);

CREATE TABLE "CtpTrade" (
    "CreatedAt" BIGINT,
    "UpdatedAt" BIGINT,
    "ExchangeId" TEXT,
    "InstrumentId" TEXT,
    "OrderSysId" TEXT,
    "TradeId" TEXT,
    "Price" DOUBLE PRECISION,
    "Volume" INT,
    "Error" TEXT
);