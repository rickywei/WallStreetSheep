#include "ctp/utils.hpp"

#include <cstring>

#include "../common/codec.hpp"
#include "../ctp_v6.7.2/ThostFtdcUserApiStruct.h"

namespace wss {

std::shared_ptr<InstrumentData> toInstrumentData(
    CThostFtdcInstrumentField *pInstrument) {
  return std::make_shared<InstrumentData>(InstrumentData{
      .ExchangeID = EncodeUtf8("GBK", std::string(pInstrument->ExchangeID)),
      .InstrumentName =
          EncodeUtf8("GBK", std::string(pInstrument->InstrumentName)),
      .ProductClass = std::string(1, pInstrument->ProductClass),
      .DeliveryYear = pInstrument->DeliveryYear,
      .DeliveryMonth = pInstrument->DeliveryMonth,
      .MaxMarketOrderVolume = pInstrument->MaxMarketOrderVolume,
      .MinMarketOrderVolume = pInstrument->MinMarketOrderVolume,
      .MaxLimitOrderVolume = pInstrument->MaxLimitOrderVolume,
      .MinLimitOrderVolume = pInstrument->MinLimitOrderVolume,
      .VolumeMultiple = pInstrument->VolumeMultiple,
      .PriceTick = pInstrument->PriceTick,
      .CreateDate = pInstrument->CreateDate,
      .OpenDate = pInstrument->OpenDate,
      .ExpireDate = pInstrument->ExpireDate,
      .StartDelivDate = pInstrument->StartDelivDate,
      .EndDelivDate = pInstrument->EndDelivDate,
      .InstLifePhase = std::string(1, pInstrument->InstLifePhase),
      .IsTrading = pInstrument->IsTrading,
      .PositionType = std::string(1, pInstrument->PositionType),
      .PositionDateType = std::string(1, pInstrument->PositionDateType),
      .LongMarginRatio = pInstrument->LongMarginRatio,
      .ShortMarginRatio = pInstrument->ShortMarginRatio,
      .MaxMarginSideAlgorithm =
          std::string(1, pInstrument->MaxMarginSideAlgorithm),
      .StrikePrice = pInstrument->StrikePrice,
      .OptionsType = std::string(1, pInstrument->InstLifePhase),
      .UnderlyingMultiple = pInstrument->UnderlyingMultiple,
      .CombinationType = std::string(1, pInstrument->InstLifePhase),
      .InstrumentID = pInstrument->InstrumentID,
      .ExchangeInstID = pInstrument->ExchangeInstID,
      .ProductID = pInstrument->ProductID,
      .UnderlyingInstrID = pInstrument->UnderlyingInstrID,
  });
}

std::shared_ptr<MarketData> toMarketData(
    CThostFtdcDepthMarketDataField *pDepthMarketData) {
  return std::make_shared<MarketData>(MarketData{
      .TradingDay =
          EncodeUtf8("GBK", std::string(pDepthMarketData->TradingDay)),
      .ExchangeID =
          EncodeUtf8("GBK", std::string(pDepthMarketData->ExchangeID)),
      .LastPrice = pDepthMarketData->LastPrice,
      .PreSettlementPrice = pDepthMarketData->PreSettlementPrice,
      .PreClosePrice = pDepthMarketData->PreClosePrice,
      .PreOpenInterest = pDepthMarketData->PreOpenInterest,
      .OpenPrice = pDepthMarketData->OpenPrice,
      .HighestPrice = pDepthMarketData->HighestPrice,
      .LowestPrice = pDepthMarketData->LowestPrice,
      .Volume = pDepthMarketData->Volume,
      .Turnover = pDepthMarketData->Turnover,
      .OpenInterest = pDepthMarketData->OpenInterest,
      .ClosePrice = pDepthMarketData->ClosePrice,
      .SettlementPrice = pDepthMarketData->SettlementPrice,
      .UpperLimitPrice = pDepthMarketData->UpperLimitPrice,
      .LowerLimitPrice = pDepthMarketData->LowerLimitPrice,
      .PreDelta = pDepthMarketData->PreDelta,
      .CurrDelta = pDepthMarketData->CurrDelta,
      .UpdateTime =
          EncodeUtf8("GBK", std::string(pDepthMarketData->UpdateTime)),
      .UpdateMillisec = pDepthMarketData->UpdateMillisec,
      .BidPrice1 = pDepthMarketData->BidPrice1,
      .BidVolume1 = pDepthMarketData->BidVolume1,
      .AskPrice1 = pDepthMarketData->AskPrice1,
      .AskVolume1 = pDepthMarketData->AskVolume1,
      .BidPrice2 = pDepthMarketData->BidPrice2,
      .BidVolume2 = pDepthMarketData->BidVolume2,
      .AskPrice2 = pDepthMarketData->AskPrice2,
      .AskVolume2 = pDepthMarketData->AskVolume2,
      .BidPrice3 = pDepthMarketData->BidPrice3,
      .BidVolume3 = pDepthMarketData->BidVolume3,
      .AskPrice3 = pDepthMarketData->AskPrice3,
      .AskVolume3 = pDepthMarketData->AskVolume3,
      .BidPrice4 = pDepthMarketData->BidPrice4,
      .BidVolume4 = pDepthMarketData->BidVolume4,
      .AskPrice4 = pDepthMarketData->AskPrice4,
      .AskVolume4 = pDepthMarketData->AskVolume4,
      .BidPrice5 = pDepthMarketData->BidPrice5,
      .BidVolume5 = pDepthMarketData->BidVolume5,
      .AskPrice5 = pDepthMarketData->AskPrice5,
      .AskVolume5 = pDepthMarketData->AskVolume5,
      .AveragePrice = pDepthMarketData->AveragePrice,
      .ActionDay = EncodeUtf8("GBK", std::string(pDepthMarketData->ActionDay)),
      .InstrumentID =
          EncodeUtf8("GBK", std::string(pDepthMarketData->InstrumentID)),
      .ExchangeInstID =
          EncodeUtf8("GBK", std::string(pDepthMarketData->ExchangeInstID)),
      .BandingUpperPrice = pDepthMarketData->BandingUpperPrice,
      .BandingLowerPrice = pDepthMarketData->BandingLowerPrice,
  });
}

}  // namespace wss