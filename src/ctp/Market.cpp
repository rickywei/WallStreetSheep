#include "WallStreetSheep/ctp/Market.hpp"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <iostream>
#include <nlohmann/json.hpp>
#include <range/v3/view/enumerate.hpp>

#include "WallStreetSheep/common/codec.hpp"
#include "WallStreetSheep/common/thread.hpp"
#include "WallStreetSheep/ctp/utils.hpp"

namespace wss {

Market::Market(std::string configPath) : ICtp(configPath) { init(); }

Market::~Market() { _mdApi->Release(); }

void Market::init() {
  YAML::Node config = YAML::LoadFile(_configPath);
  YAML::Node market_config = config["market"];
  _frontAddr = market_config["front_addr"].as<std::string>();
  _brokerId = market_config["broker_id"].as<std::string>();
  _investorId = market_config["investor_id"].as<std::string>();
  _password = market_config["password"].as<std::string>();
  _flowPath = market_config["flow_path"].as<std::string>();
  _isUsingUdp = market_config["is_using_udp"].as<bool>();
  _isMulticast = market_config["is_multicast"].as<bool>();
}

void Market::start() {
  SPDLOG_INFO("start ctp market, version={0}", _mdApi->GetApiVersion());
  _mdApi = CThostFtdcMdApi::CreateFtdcMdApi(_flowPath.c_str(), _isUsingUdp,
                                            _isMulticast);
  _mdApi->RegisterSpi(this);
  _mdApi->RegisterFront(const_cast<char *>(_frontAddr.c_str()));
  _mdApi->Init();
  _logged.wait(false);
  SPDLOG_INFO("inited");
  postTask([this]() {
    int ret = this->_mdApi->Join();
    SPDLOG_INFO("join returned, ret={}", ret);
  });
}

int Market::subscribe(std::unordered_set<std::string> &instrumentIds) {
  size_t n = instrumentIds.size();
  auto ppInstrumentID = std::make_unique<char *[]>(n);
  for (const auto &[i, s] : instrumentIds | ranges::views::enumerate) {
    ppInstrumentID[i] = const_cast<char *>(s.c_str());
  }
  int ret = _mdApi->SubscribeMarketData(ppInstrumentID.get(), n);
  if (ret != 0) SPDLOG_ERROR("ret = {}", ret);
  return ret;
}

int Market::unsubscribe(std::unordered_set<std::string> &instrumentIds) {
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

void Market::OnFrontConnected() {
  if (int ret = login(); ret != 0) {
    SPDLOG_ERROR("ret = {}", ret);
    return;
  }
  _logged.store(true, std::memory_order_release);
  _logged.notify_one();
}

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
  _logged.store(true, std::memory_order_release);
  _logged.notify_one();
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
  auto sp =
      deepCopyToSharedPtr<CThostFtdcDepthMarketDataField>(pDepthMarketData);
  _db->write(
      R"(INSERT INTO
        Market(TradingDay, ExchangeID,  LastPrice,
                  PreSettlementPrice, PreClosePrice, PreOpenInterest, OpenPrice,
                  HighestPrice, LowestPrice, Volume, Turnover, OpenInterest,
                  ClosePrice, SettlementPrice, UpperLimitPrice, LowerLimitPrice,
                  PreDelta, CurrDelta, UpdateTime, UpdateMillisec, BidPrice1,
                  BidVolume1, AskPrice1, AskVolume1, BidPrice2, BidVolume2,
                  AskPrice2, AskVolume2, BidPrice3, BidVolume3, AskPrice3,
                  AskVolume3, BidPrice4, BidVolume4, AskPrice4, AskVolume4,
                  BidPrice5, BidVolume5, AskPrice5, AskVolume5, AveragePrice,
                  ActionDay, InstrumentID, ExchangeInstID, BandingUpperPrice,
                  BandingLowerPrice)
            VALUES($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13, $14,
                   $15, $16, $17, $18, $19, $20, $21, $22, $23, $24, $25, $26,
                   $27, $28, $29, $30, $31, $32, $33, $34, $35, $36, $37, $38,
                   $39, $40, $41, $42, $43, $44, $45, $46)
                ON CONFLICT DO NOTHING)",
      sp->TradingDay, sp->ExchangeID, sp->LastPrice, sp->PreSettlementPrice,
      sp->PreClosePrice, sp->PreOpenInterest, sp->OpenPrice, sp->HighestPrice,
      sp->LowestPrice, sp->Volume, sp->Turnover, sp->OpenInterest,
      sp->ClosePrice, sp->SettlementPrice, sp->UpperLimitPrice,
      sp->LowerLimitPrice, sp->PreDelta, sp->CurrDelta, sp->UpdateTime,
      sp->UpdateMillisec, sp->BidPrice1, sp->BidVolume1, sp->AskPrice1,
      sp->AskVolume1, sp->BidPrice2, sp->BidVolume2, sp->AskPrice2,
      sp->AskVolume2, sp->BidPrice3, sp->BidVolume3, sp->AskPrice3,
      sp->AskVolume3, sp->BidPrice4, sp->BidVolume4, sp->AskPrice4,
      sp->AskVolume4, sp->BidPrice5, sp->BidVolume5, sp->AskPrice5,
      sp->AskVolume5, sp->AveragePrice, sp->ActionDay, sp->InstrumentID,
      sp->ExchangeInstID, sp->BandingUpperPrice, sp->BandingLowerPrice);
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

int Market::login() {
  CThostFtdcReqUserLoginField req;
  memset(&req, 0, sizeof(req));
  strcpy(req.BrokerID, _brokerId.c_str());
  strcpy(req.UserID, _investorId.c_str());
  strcpy(req.Password, _password.c_str());

  return _mdApi->ReqUserLogin(&req, ++_requestId);
}

}  // namespace wss
