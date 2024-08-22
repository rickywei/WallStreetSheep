#include "market.hpp"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <nlohmann/json.hpp>
#include <range/v3/all.hpp>
#include <vector>

#include "codec.hpp"
#include "thread.hpp"

namespace wss {

constexpr std::string_view kFmtRequestId = "ctpRequestId:{}";
constexpr std::string_view chCtpHeartbeat = "chCtpHeartbeat";
constexpr std::string_view chCtpSubs = "chCtpSubs";
constexpr std::string_view chCtpUnsubs = "chCtpUnsubs";
constexpr std::string_view chCtpMarket = "chCtpMarket";

Market::Market(std::string str) {
  auto j = nlohmann::json::parse(str);
  j.at("marketFrontAddr").get_to(_frontAddr);
  j.at("brokerId").get_to(_brokerId);
  j.at("investorId").get_to(_investorId);
  j.at("password").get_to(_password);
  j.at("redisAddr").get_to(_redisAddr);
  j.at("flowPath").get_to(_flowPath);
  j.at("isUsingUdp").get_to(_isUsingUdp);
  j.at("isMulticast").get_to(_isMulticast);
  _rc = std::make_unique<sw::redis::Redis>(_redisAddr);
}

Market::~Market() { _mdApi->Release(); }

void Market::start() {
  SPDLOG_INFO("version={}", _mdApi->GetApiVersion());
  _mdApi = CThostFtdcMdApi::CreateFtdcMdApi(_flowPath.c_str(), _isUsingUdp,
                                            _isMulticast);
  _mdApi->RegisterSpi(this);
  _mdApi->RegisterFront(const_cast<char *>(_frontAddr.c_str()));
  _mdApi->Init();
  _daemon();
  SPDLOG_INFO("inited");
  postTask([this]() {
    int ret = this->_mdApi->Join();
    SPDLOG_INFO("join returned, ret={}", ret);
  });
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
  SPDLOG_INFO("nReason={}", nReason);
  _isLogin = false;
  start();
}

void Market::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                            CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                            bool bIsLast) {
  if (pRspInfo != nullptr && pRspInfo->ErrorID != 0) {
    SPDLOG_ERROR("nRequest={}, code={}, msg={}", nRequestID, pRspInfo->ErrorID,
                 EncodeUtf8("GBK", std::string(pRspInfo->ErrorMsg)));
    return;
  }
  _isLogin = true;
}

void Market::OnRspSubMarketData(
    CThostFtdcSpecificInstrumentField *pSpecificInstrument,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  if (pRspInfo != nullptr && pRspInfo->ErrorID != 0) {
    SPDLOG_ERROR("nRequest={}, code={}, msg={}", nRequestID, pRspInfo->ErrorID,
                 EncodeUtf8("GBK", std::string(pRspInfo->ErrorMsg)));
    return;
  }
}

void Market::OnRtnDepthMarketData(
    CThostFtdcDepthMarketDataField *pDepthMarketData) {
  nlohmann::json j{
      {"TradingDay",
       EncodeUtf8("GBK", std::string(pDepthMarketData->TradingDay))},
      // {"ExchangeID",
      //  EncodeUtf8("GBK", std::string(pDepthMarketData->ExchangeID))},
      {"LastPrice", pDepthMarketData->LastPrice},
      {"PreSettlementPrice", pDepthMarketData->PreSettlementPrice},
      {"PreClosePrice", pDepthMarketData->PreClosePrice},
      {"PreOpenInterest", pDepthMarketData->PreOpenInterest},
      {"OpenPrice", pDepthMarketData->OpenPrice},
      {"HighestPrice", pDepthMarketData->HighestPrice},
      {"LowestPrice", pDepthMarketData->LowestPrice},
      {"Volume", pDepthMarketData->Volume},
      {"Turnover", pDepthMarketData->Turnover},
      {"OpenInterest", pDepthMarketData->OpenInterest},
      {"ClosePrice", pDepthMarketData->ClosePrice},
      {"SettlementPrice", pDepthMarketData->SettlementPrice},
      {"UpperLimitPrice", pDepthMarketData->UpperLimitPrice},
      {"LowerLimitPrice", pDepthMarketData->LowerLimitPrice},
      {"PreDelta", pDepthMarketData->PreDelta},
      {"CurrDelta", pDepthMarketData->CurrDelta},
      {"UpdateTime",
       EncodeUtf8("GBK", std::string(pDepthMarketData->UpdateTime))},
      {"UpdateMillisec", pDepthMarketData->UpdateMillisec},
      {"BidPrice1", pDepthMarketData->BidPrice1},
      {"BidVolume1", pDepthMarketData->BidVolume1},
      {"AskPrice1", pDepthMarketData->AskPrice1},
      {"AskVolume1", pDepthMarketData->AskVolume1},
      // {"BidPrice2", pDepthMarketData->BidPrice2},
      // {"BidVolume2", pDepthMarketData->BidVolume2},
      // {"AskPrice2", pDepthMarketData->AskPrice2},
      // {"AskVolume2", pDepthMarketData->AskVolume2},
      // {"BidPrice3", pDepthMarketData->BidPrice3},
      // {"BidVolume3", pDepthMarketData->BidVolume3},
      // {"AskPrice3", pDepthMarketData->AskPrice3},
      // {"AskVolume3", pDepthMarketData->AskVolume3},
      // {"BidPrice4", pDepthMarketData->BidPrice4},
      // {"BidVolume4", pDepthMarketData->BidVolume4},
      // {"AskPrice4", pDepthMarketData->AskPrice4},
      // {"AskVolume4", pDepthMarketData->AskVolume4},
      // {"BidPrice5", pDepthMarketData->BidPrice5},
      // {"BidVolume5", pDepthMarketData->BidVolume5},
      // {"AskPrice5", pDepthMarketData->AskPrice5},
      // {"AskVolume5", pDepthMarketData->AskVolume5},
      {"AveragePrice", pDepthMarketData->AveragePrice},
      {"ActionDay",
       EncodeUtf8("GBK", std::string(pDepthMarketData->ActionDay))},
      {"InstrumentID",
       EncodeUtf8("GBK", std::string(pDepthMarketData->InstrumentID))},
      // {"ExchangeInstID",
      //  EncodeUtf8("GBK", std::string(pDepthMarketData->ExchangeInstID))},
      // {"BandingUpperPrice", pDepthMarketData->BandingUpperPrice},
      // {"BandingLowerPrice", pDepthMarketData->BandingLowerPrice},
  };

  _rc->publish(chCtpMarket, j.dump());
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

int Market::_getRequestId() {
  return _rc->incr(fmt::format(kFmtRequestId, _mdApi->GetTradingDay()));
}

void Market::_daemon() {
  // heartbeat
  postTask([this]() {
    while (true) {
      if (this->_isLogin.load()) {
        _rc->publish(chCtpHeartbeat, "md");
      }
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  });

  postTask([this]() {
    auto ch = this->_rc->subscriber();
    ch.subscribe(chCtpSubs);
    ch.subscribe(chCtpUnsubs);
    ch.on_message([this](std::string channel, std::string msg) {
      if (channel == chCtpSubs)
        this->_subscribe(msg);
      else if (channel == chCtpUnsubs)
        this->_unsubscribe(msg);
    });
    while (true) {
      try {
        ch.consume();
      } catch (std::exception &e) {
        SPDLOG_ERROR("{}", e.what());
      }
    }
  });
}

void Market::_login() {
  CThostFtdcReqUserLoginField req;
  memset(&req, 0, sizeof(req));
  strcpy(req.BrokerID, _brokerId.c_str());
  strcpy(req.UserID, _investorId.c_str());
  strcpy(req.Password, _password.c_str());
  if (int ret = _mdApi->ReqUserLogin(&req, _getRequestId()); ret != 0) {
    SPDLOG_ERROR("ret = {}", ret);
  }
}

void Market::_subscribe(std::string str) {
  SPDLOG_INFO("str={}", str);
  try {
    auto j = nlohmann::json::parse(str);
    auto instrumentIds = j.get<std::vector<std::string>>();
    size_t n = instrumentIds.size();
    auto ppInstrumentID = std::make_unique<char *[]>(n);
    for (const auto &[i, s] : instrumentIds | ranges::views::enumerate) {
      ppInstrumentID[i] = const_cast<char *>(s.c_str());
    }
    if (int ret = _mdApi->SubscribeMarketData(ppInstrumentID.get(), n);
        ret != 0) {
      SPDLOG_ERROR("ret = {}", ret);
    }
  } catch (std::exception &e) {
    SPDLOG_ERROR("exception={}", e.what());
  }
}

void Market::_unsubscribe(std::string str) {
  SPDLOG_INFO("str={}", str);
  try {
    auto j = nlohmann::json::parse(str);
    auto instrumentIds = j.get<std::vector<std::string>>();
    size_t n = instrumentIds.size();
    auto ppInstrumentID = std::make_unique<char *[]>(n);
    for (const auto &[i, s] : instrumentIds | ranges::views::enumerate) {
      ppInstrumentID[i] = const_cast<char *>(s.c_str());
    }
    if (int ret = _mdApi->UnSubscribeMarketData(ppInstrumentID.get(), n);
        ret != 0) {
      SPDLOG_ERROR("ret = {}", ret);
    }
  } catch (std::exception &e) {
    SPDLOG_ERROR("exception={}", e.what());
  }
}

}  // namespace wss