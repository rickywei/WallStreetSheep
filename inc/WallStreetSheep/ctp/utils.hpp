#pragma once

#include <memory>

#include "api/ctp_v6.7.2/ThostFtdcUserApiStruct.h"

namespace wss {

enum class Mode { None, FAK, FOK };

inline bool isValidCondition(TThostFtdcContingentConditionType condition);

template <typename T>
std::shared_ptr<T> deepCopyToSharedPtr(T *p);

template <>
std::shared_ptr<CThostFtdcInstrumentField> deepCopyToSharedPtr(
    CThostFtdcInstrumentField *pInstrument);

template <>
std::shared_ptr<CThostFtdcDepthMarketDataField> deepCopyToSharedPtr(
    CThostFtdcDepthMarketDataField *pDepthMarketData);

}  // namespace wss
