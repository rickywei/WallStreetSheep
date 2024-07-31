#include "alphamaker/market/market_ctp.hpp"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <iostream>
#include <nlohmann/json.hpp>

#include "alphamaker/common/common.hpp"
#include "convert/codec.hpp"

namespace am {
void MarketCtp::init() {
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

void MarketCtp::start() {
  SPDLOG_INFO("start ctp market, version={0}", _mdApi->GetApiVersion());
  _mdApi = CThostFtdcMdApi::CreateFtdcMdApi(_flowPath.c_str(), _isUsingUdp,
                                            _isMulticast);
  _mdApi->RegisterSpi(this);
  _mdApi->RegisterFront(const_cast<char *>(_frontAddr.c_str()));
  _mdApi->Init();
  int ret = _mdApi->Join();
  SPDLOG_INFO("join returned, ret={}", ret);
}

void MarketCtp::disconnect() {
  SPDLOG_INFO("reconnect");
  start();
}

void MarketCtp::subscribe() {
  SPDLOG_INFO("subscribe");
  // TODO how to get all instrument ids
  char *ppInstrumentID[]{
      "au2410",
  };
  _mdApi->SubscribeMarketData(ppInstrumentID, 1);
}

void MarketCtp::unsubscribe() { SPDLOG_INFO("unsubscribe"); }

void MarketCtp::OnFrontConnected() {
  if (int ret = login(); ret != 0) {
    SPDLOG_ERROR("login failed...ret = {}", ret);
    return;
  }
}

void MarketCtp::OnFrontDisconnected(int nReason) {
  SPDLOG_INFO("OnFrontDisconnected...{0}", nReason);
}

void MarketCtp::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                               CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                               bool bIsLast) {
  SPDLOG_INFO("OnRspUserLogin...code={0} msg={0}", pRspInfo->ErrorID,
              pRspInfo->ErrorMsg);
  if (pRspInfo->ErrorID != 0) return;
  subscribe();
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
  SPDLOG_INFO("code={0}, msg={1}", pRspInfo->ErrorID,
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
  postTask([pDepthMarketData, this]() {
    SPDLOG_INFO("xxxxxxxxxxxxxxxx,updatetime-mil={}-{}",
                pDepthMarketData->UpdateTime, pDepthMarketData->UpdateMillisec);
    try {
      auto work = this->_db->newWork();
      work->exec_params(
          "INSERT INTO ctpMarket (TradingDay, reserve1, ExchangeID, reserve2, "
          "LastPrice, PreSettlementPrice, PreClosePrice, PreOpenInterest, "
          "OpenPrice, HighestPrice, LowestPrice, Volume, Turnover, "
          "OpenInterest, "
          "ClosePrice, SettlementPrice, UpperLimitPrice, LowerLimitPrice, "
          "PreDelta, CurrDelta, UpdateTime, UpdateMillisec, BidPrice1, "
          "BidVolume1, "
          "AskPrice1, AskVolume1, BidPrice2, BidVolume2, AskPrice2, "
          "AskVolume2, "
          "BidPrice3, BidVolume3, AskPrice3, AskVolume3, BidPrice4, "
          "BidVolume4, "
          "AskPrice4, AskVolume4, BidPrice5, BidVolume5, AskPrice5, "
          "AskVolume5, "
          "AveragePrice, ActionDay, InstrumentID, ExchangeInstID, "
          "BandingUpperPrice, BandingLowerPrice) VALUES "
          "($1,$2,$3,$4,$5,$6,$7,$8,$9,$10,$11,$12,$13,$14,$15,$16,$17,$18,$19,"
          "$"
          "20,"
          "$21,$22,$23,$24,$25,$26,$27,$28,$29,$30,$31,$32,$33,$34,$35,$36,$37,"
          "$"
          "38,"
          "$39,$40,$41,$42,$43,$44,$45,$46,$47,$48)",
          pDepthMarketData->TradingDay, pDepthMarketData->reserve1,
          pDepthMarketData->ExchangeID, pDepthMarketData->reserve2,
          pDepthMarketData->LastPrice, pDepthMarketData->PreSettlementPrice,
          pDepthMarketData->PreClosePrice, pDepthMarketData->PreOpenInterest,
          pDepthMarketData->OpenPrice, pDepthMarketData->HighestPrice,
          pDepthMarketData->LowestPrice, pDepthMarketData->Volume,
          pDepthMarketData->Turnover, pDepthMarketData->OpenInterest,
          pDepthMarketData->ClosePrice, pDepthMarketData->SettlementPrice,
          pDepthMarketData->UpperLimitPrice, pDepthMarketData->LowerLimitPrice,
          pDepthMarketData->PreDelta, pDepthMarketData->CurrDelta,
          pDepthMarketData->UpdateTime, pDepthMarketData->UpdateMillisec,
          pDepthMarketData->BidPrice1, pDepthMarketData->BidVolume1,
          pDepthMarketData->AskPrice1, pDepthMarketData->AskVolume1,
          pDepthMarketData->BidPrice2, pDepthMarketData->BidVolume2,
          pDepthMarketData->AskPrice2, pDepthMarketData->AskVolume2,
          pDepthMarketData->BidPrice3, pDepthMarketData->BidVolume3,
          pDepthMarketData->AskPrice3, pDepthMarketData->AskVolume3,
          pDepthMarketData->BidPrice4, pDepthMarketData->BidVolume4,
          pDepthMarketData->AskPrice4, pDepthMarketData->AskVolume4,
          pDepthMarketData->BidPrice5, pDepthMarketData->BidVolume5,
          pDepthMarketData->AskPrice5, pDepthMarketData->AskVolume5,
          pDepthMarketData->AveragePrice, pDepthMarketData->ActionDay,
          pDepthMarketData->InstrumentID, pDepthMarketData->ExchangeInstID,
          pDepthMarketData->BandingUpperPrice,
          pDepthMarketData->BandingLowerPrice);
      work->commit();
    } catch (std::exception &e) {
      SPDLOG_ERROR("save ctp market data failed, exception={}", e.what());
    }
  });
}

void MarketCtp::OnRtnForQuoteRsp(CThostFtdcForQuoteRspField *pForQuoteRsp) {}

MarketCtp::MarketCtp(std::string configPath) : Market(configPath) { init(); }

MarketCtp::~MarketCtp() { _mdApi->Release(); }

int MarketCtp::login() {
  SPDLOG_INFO("login");

  CThostFtdcReqUserLoginField req;
  memset(&req, 0, sizeof(req));
  strcpy(req.BrokerID, _brokerId.c_str());
  strcpy(req.UserID, _investorId.c_str());
  strcpy(req.Password, _password.c_str());

  return _mdApi->ReqUserLogin(&req, ++_requestId);
}

}  // namespace am
