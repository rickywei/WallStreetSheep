#include "WallStreetSheep/ctp/utils.hpp"

#include <cstring>

#include "api/ctp_v6.7.2/ThostFtdcUserApiStruct.h"

namespace wss {

template <typename T>
std::shared_ptr<T> deepCopyToSharedPtr(T *p) {
  return std::make_shared<T>(*p);
}

template <>
std::shared_ptr<CThostFtdcInstrumentField> deepCopyToSharedPtr(
    CThostFtdcInstrumentField *pInstrument) {
  auto sp = std::make_shared<CThostFtdcInstrumentField>(*pInstrument);
  strcpy(sp->ExchangeID, pInstrument->ExchangeID);
  strcpy(sp->InstrumentName, pInstrument->InstrumentName);
  strcpy(sp->CreateDate, pInstrument->CreateDate);
  strcpy(sp->OpenDate, pInstrument->OpenDate);
  strcpy(sp->ExpireDate, pInstrument->ExpireDate);
  strcpy(sp->StartDelivDate, pInstrument->StartDelivDate);
  strcpy(sp->EndDelivDate, pInstrument->EndDelivDate);
  strcpy(sp->InstrumentID, pInstrument->InstrumentID);
  strcpy(sp->ExchangeInstID, pInstrument->ExchangeInstID);
  strcpy(sp->ProductID, pInstrument->ProductID);
  strcpy(sp->UnderlyingInstrID, pInstrument->UnderlyingInstrID);
  return sp;
}

template <>
std::shared_ptr<CThostFtdcDepthMarketDataField> deepCopyToSharedPtr(
    CThostFtdcDepthMarketDataField *pDepthMarketData) {
  auto sp = std::make_shared<CThostFtdcDepthMarketDataField>(*pDepthMarketData);
  strcpy(sp->TradingDay, pDepthMarketData->TradingDay);
  strcpy(sp->ExchangeID, pDepthMarketData->ExchangeID);
  strcpy(sp->UpdateTime, pDepthMarketData->UpdateTime);
  strcpy(sp->ActionDay, pDepthMarketData->ActionDay);
  strcpy(sp->InstrumentID, pDepthMarketData->InstrumentID);
  strcpy(sp->ExchangeInstID, pDepthMarketData->ExchangeInstID);
  return sp;
}

bool isValidCondition(TThostFtdcContingentConditionType condition) {
  return condition == THOST_FTDC_CC_LastPriceGreaterThanStopPrice ||
         condition == THOST_FTDC_CC_LastPriceGreaterEqualStopPrice ||
         condition == THOST_FTDC_CC_LastPriceLesserThanStopPrice ||
         condition == THOST_FTDC_CC_LastPriceLesserEqualStopPrice ||
         condition == THOST_FTDC_CC_AskPriceGreaterThanStopPrice ||
         condition == THOST_FTDC_CC_AskPriceGreaterEqualStopPrice ||
         condition == THOST_FTDC_CC_AskPriceLesserThanStopPrice ||
         condition == THOST_FTDC_CC_AskPriceLesserEqualStopPrice ||
         condition == THOST_FTDC_CC_BidPriceGreaterThanStopPrice ||
         condition == THOST_FTDC_CC_BidPriceGreaterEqualStopPrice ||
         condition == THOST_FTDC_CC_BidPriceLesserThanStopPrice ||
         condition == THOST_FTDC_CC_BidPriceLesserEqualStopPrice;
}

}  // namespace wss
