#include "alphamaker/market/market_ctp.hpp"

#include <spdlog/spdlog.h>

#include <nlohmann/json.hpp>

namespace am {
bool MarketCtp::init() {
  // _md_api->RegisterSpi(this);
  _md_api->RegisterFront(const_cast<char *>(_front_addr.c_str()));
  _md_api->Init();
  _md_api->Join();
}

void MarketCtp::release() { _md_api->Release(); }

void MarketCtp::connect() {
  if (int ret = Login(); ret != 0) {
    spdlog::error("login failed...ret = {}", ret);
    return;
  }
}

void MarketCtp::disconnect() {}

void MarketCtp::subscribe() {
  // TODO how to get all instrument ids
  char *ppInstrumentID[]{
      "au2408",
  };
  _md_api->SubscribeMarketData(ppInstrumentID, 1);
}

void MarketCtp::unsubscribe() {}

void MarketCtp::OnFrontConnected() {
  spdlog::info("OnFrontConnected...");
  Login();
}

void MarketCtp::OnFrontDisconnected(int nReason) {}

void MarketCtp::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                               CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                               bool bIsLast) {
  connect();
}

void MarketCtp::OnHeartBeatWarning(int nTimeLapse) {}

void MarketCtp::OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
                                CThostFtdcRspInfoField *pRspInfo,
                                int nRequestID, bool bIsLast) {}

void MarketCtp::OnRspQryMulticastInstrument(
    CThostFtdcMulticastInstrumentField *pMulticastInstrument,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

void MarketCtp::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                           bool bIsLast) {}

void MarketCtp::OnRspSubMarketData(
    CThostFtdcSpecificInstrumentField *pSpecificInstrument,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  spdlog::info("OnRspSubMarketData...");
}

void MarketCtp::OnRspUnSubMarketData(
    CThostFtdcSpecificInstrumentField *pSpecificInstrument,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

void MarketCtp::OnRspSubForQuoteRsp(
    CThostFtdcSpecificInstrumentField *pSpecificInstrument,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

void MarketCtp::OnRspUnSubForQuoteRsp(
    CThostFtdcSpecificInstrumentField *pSpecificInstrument,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

void MarketCtp::OnRtnDepthMarketData(
    CThostFtdcDepthMarketDataField *pDepthMarketData) {
  // using json = nlohmann::json;
  // json j = json::parse(pDepthMarketData);
  spdlog::info("xxxxx");
}

void MarketCtp::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp) {}

MarketCtp::MarketCtp(std::string frontAddr, std::string brokerId,
                     std::string investorId, std::string password,
                     std::string flowPath, bool isUsingUdp, bool isMulticast)
    : _front_addr{frontAddr},
      _broker_id{brokerId},
      _investor_id{investorId},
      _password{password},
      _flowPath{flowPath},
      _is_using_udp{isUsingUdp},
      _is_multicast{isMulticast},
      _md_api{CThostFtdcMdApi::CreateFtdcMdApi(_flowPath.c_str(), _is_using_udp,
                                               _is_multicast)} {
  // TODO read config
  init();
}

MarketCtp::~MarketCtp() { release(); }

int MarketCtp::Login() {
  CThostFtdcReqUserLoginField req;
  memset(&req, 0, sizeof(req));
  strcpy(req.BrokerID, _broker_id.c_str());
  strcpy(req.UserID, _investor_id.c_str());
  strcpy(req.Password, _password.c_str());
  return _md_api->ReqUserLogin(&req, ++_requestId);
}

bool MarketCtp::IsError(const CThostFtdcRspInfoField *pRspInfo) const {
  return pRspInfo->ErrorID != 0;
}

}  // namespace am
