#include <memory>
#include <string>

#include "../ctp_v6.7.2/ThostFtdcUserApiStruct.h"
#include "../../common/data.hpp"

namespace wss {

std::string marshal(CThostFtdcDepthMarketDataField *pDepthMarketData);
std::unique_ptr<Tick> unmarshalTick(std::string str);
std::string marshal(CThostFtdcInstrumentField *pInstrument);
}  // namespace wss