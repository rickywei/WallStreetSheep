#include "alphamaker/market/market_ctp.hpp"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <iostream>
#include <nlohmann/json.hpp>

#include "convert/codec.hpp"

namespace am {
void MarketCtp::LoadConfig() {
  YAML::Node config = YAML::LoadFile(_config_path);
  YAML::Node market_config = config["market"];
  spdlog::info("read config...\n{0}", YAML::Dump(market_config));
  _front_addr = market_config["front_addr"].as<std::string>();
  _broker_id = market_config["broker_id"].as<std::string>();
  _investor_id = market_config["investor_id"].as<std::string>();
  _password = market_config["password"].as<std::string>();
  _flowPath = market_config["flow_path"].as<std::string>();
  _is_using_udp = market_config["is_using_udp"].as<bool>();
  _is_multicast = market_config["is_multicast"].as<bool>();
}

void MarketCtp::Init() {
  spdlog::info("version={0}", _md_api->GetApiVersion());
  _md_api = CThostFtdcMdApi::CreateFtdcMdApi(_flowPath.c_str(), _is_using_udp,
                                             _is_multicast);
  _md_api->RegisterSpi(this);
}

void MarketCtp::Connect() {
  spdlog::info("connect");
  _md_api->RegisterFront(const_cast<char *>(_front_addr.c_str()));
  _md_api->Init();
  int ret = _md_api->Join();
}

void MarketCtp::Disconnect() {
  spdlog::info("Disconnect");
  Connect();
}

void MarketCtp::Subscribe() {
  spdlog::info("subscribe");

  // TODO how to get all instrument ids
  char *ppInstrumentID[]{
      "au2408C560",
  };
  _md_api->SubscribeMarketData(ppInstrumentID, 1);
}

void MarketCtp::Unsubscribe() {}

void MarketCtp::OnFrontConnected() {
  if (int ret = Login(); ret != 0) {
    spdlog::error("login failed...ret = {}", ret);
    return;
  }
}

void MarketCtp::OnFrontDisconnected(int nReason) {
  spdlog::info("OnFrontDisconnected...{0}", nReason);
}

void MarketCtp::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                               CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                               bool bIsLast) {
  spdlog::info("OnRspUserLogin...code={0} msg={0}", pRspInfo->ErrorID,
               pRspInfo->ErrorMsg);
  if (pRspInfo->ErrorID != 0) return;
  Subscribe();
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
  spdlog::info("OnRspSubMarketData...{0} {1}", pRspInfo->ErrorID,
               ::EncodeUtf8("GBK", std::string(pRspInfo->ErrorMsg)));
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
  // json j = json::parse(*pDepthMarketData);

  spdlog::info("last={0} open={1} close={2} high={3} low={4} up={5}",
               pDepthMarketData->LastPrice, pDepthMarketData->OpenPrice,
               pDepthMarketData->PreClosePrice, pDepthMarketData->HighestPrice,
               pDepthMarketData->LowestPrice,pDepthMarketData->TradingDay);
}

void MarketCtp::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp) {}

MarketCtp::MarketCtp(std::string config_path) : Market(config_path) {
  LoadConfig();
}

MarketCtp::~MarketCtp() { _md_api->Release(); }

int MarketCtp::Login() {
  spdlog::info("login");

  CThostFtdcReqUserLoginField req;
  memset(&req, 0, sizeof(req));
  strcpy(req.BrokerID, _broker_id.c_str());
  strcpy(req.UserID, _investor_id.c_str());
  strcpy(req.Password, _password.c_str());

  spdlog::info("req.broker_id={0}", req.BrokerID);
  spdlog::info("req.userid={0}", req.UserID);
  spdlog::info("req.password={0}", req.Password);

  return _md_api->ReqUserLogin(&req, ++_requestId);
}

bool MarketCtp::IsError(const CThostFtdcRspInfoField *pRspInfo) const {
  return pRspInfo->ErrorID != 0;
}

}  // namespace am
