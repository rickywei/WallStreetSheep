#include "../market/Market.hpp"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <iostream>
#include <range/v3/all.hpp>

#include "../../common/codec.hpp"
#include "../../common/thread.hpp"
#include "../encoding/json.hpp"
#include "Market.hpp"

namespace wss {

constexpr std::string chInstrument = "ctpMarket";
constexpr std::string chSubs = "ctpSubs";
constexpr std::string chFmtSubs = "ctpSubs:{}";

Market::Market() {
  init();
  start();
}

Market::~Market() { _mdApi->Release(); }

void Market::init() {
  YAML::Node conf = YAML::LoadFile("./conf.yaml");
  _frontAddr = conf["front_addr"].as<std::string>();
  _brokerId = conf["broker_id"].as<std::string>();
  _investorId = conf["investor_id"].as<std::string>();
  _password = conf["password"].as<std::string>();
  _flowPath = conf["flow_path"].as<std::string>();
  _isUsingUdp = conf["is_using_udp"].as<bool>();
  _isMulticast = conf["is_multicast"].as<bool>();

  _rc = std::make_unique<sw::redis::Redis>(conf["redisAddr"].as<std::string>());
}

void Market::start() {
  SPDLOG_INFO("start ctp market, version={0}", _mdApi->GetApiVersion());
  _mdApi = CThostFtdcMdApi::CreateFtdcMdApi(_flowPath.c_str(), _isUsingUdp,
                                            _isMulticast);
  _mdApi->RegisterSpi(this);
  _mdApi->RegisterFront(const_cast<char *>(_frontAddr.c_str()));
  _mdApi->Init();
  SPDLOG_INFO("inited");
  postTask([this]() {
    int ret = this->_mdApi->Join();
    SPDLOG_INFO("join returned, ret={}", ret);
  });
}

int Market::subscribe(std::vector<std::string> &instrumentIds) {
  size_t n = instrumentIds.size();
  auto ppInstrumentID = std::make_unique<char *[]>(n);
  for (const auto &[i, s] : instrumentIds | ranges::views::enumerate) {
    ppInstrumentID[i] = const_cast<char *>(s.c_str());
  }
  int ret = _mdApi->SubscribeMarketData(ppInstrumentID.get(), n);
  if (ret != 0) SPDLOG_ERROR("ret = {}", ret);
  return ret;
}

int Market::unsubscribe(std::vector<std::string> &instrumentIds) {
  size_t n = instrumentIds.size();
  auto ppInstrumentID = std::make_unique<char *[]>(n);
  for (const auto &[i, s] : instrumentIds | ranges::views::enumerate) {
    ppInstrumentID[i] = const_cast<char *>(s.c_str());
  }
  int ret = _mdApi->UnSubscribeMarketData(ppInstrumentID.get(), n);
  if (ret != 0) SPDLOG_ERROR("ret = {}", ret);
  return ret;
}

void Market::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                        bool bIsLast) {
  if (pRspInfo != nullptr && pRspInfo->ErrorID != 0) {
    SPDLOG_ERROR("code={}, msg={}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    return;
  }
}

void Market::OnFrontConnected() { _login(); }

void Market::OnFrontDisconnected(int nReason) {
  SPDLOG_INFO("nReason={0}", nReason);
}

void Market::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                            CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                            bool bIsLast) {
  if (pRspInfo != nullptr && pRspInfo->ErrorID != 0) {
    SPDLOG_ERROR("code={}, msg={}", pRspInfo->ErrorID,
                 EncodeUtf8("GBK", std::string(pRspInfo->ErrorMsg)));
    return;
  }
  postTask([this]() {
    std::vector<std::string> v;
    _rc->smembers(fmt::format(chFmtSubs, Date()), std::inserter(v, v.begin()));
    this->subscribe(v);

    auto ch = _rc->subscriber();
    ch.subscribe(chSubs);
    ch.on_message([this](std::string channel, std::string msg) {
      SPDLOG_INFO("instrumentId={}", msg);
      std::vector<std::string> v{msg};
      this->subscribe(v);
    });
    while (true) {
      ch.consume();
    }
  });
}

void Market::OnRspSubMarketData(
    CThostFtdcSpecificInstrumentField *pSpecificInstrument,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  if (pRspInfo != nullptr && pRspInfo->ErrorID != 0) {
    SPDLOG_ERROR("code={}, msg={}", pRspInfo->ErrorID,
                 EncodeUtf8("GBK", std::string(pRspInfo->ErrorMsg)));
    return;
  }
}

void Market::OnRtnDepthMarketData(
    CThostFtdcDepthMarketDataField *pDepthMarketData) {
  _rc->publish(chInstrument, marshal(pDepthMarketData));
}

void Market::OnRspUnSubMarketData(
    CThostFtdcSpecificInstrumentField *pSpecificInstrument,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  if (pRspInfo != nullptr && pRspInfo->ErrorID != 0) {
    SPDLOG_ERROR("code={}, msg={}", pRspInfo->ErrorID,
                 EncodeUtf8("GBK", std::string(pRspInfo->ErrorMsg)));
    return;
  }
}

int Market::_login() {
  CThostFtdcReqUserLoginField req;
  memset(&req, 0, sizeof(req));
  strcpy(req.BrokerID, _brokerId.c_str());
  strcpy(req.UserID, _investorId.c_str());
  strcpy(req.Password, _password.c_str());
  int ret = _mdApi->ReqUserLogin(&req, ++_requestId);
  if (ret != 0) SPDLOG_ERROR("ret = {}", ret);
  return ret;
}

}  // namespace wss
