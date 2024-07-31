#include "WallStreetSheep/trade/TradeCtp.hpp"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <iostream>
#include <nlohmann/json.hpp>

#include "convert/codec.hpp"

namespace wss {
void TradeCtp::init() {
  YAML::Node config = YAML::LoadFile(_config_path);
  YAML::Node trade_config = config["trade"];
  _frontAddr = trade_config["front_addr"].as<std::string>();
  _brokerId = trade_config["broker_id"].as<std::string>();
  _investorId = trade_config["investor_id"].as<std::string>();
  _password = trade_config["password"].as<std::string>();
  _flowPath = trade_config["flow_path"].as<std::string>();
  SPDLOG_INFO("read ctp trade config...\n{0}", YAML::Dump(trade_config));
}

void TradeCtp::start() {
  SPDLOG_INFO("version={}", _tdApi->GetApiVersion());
  _tdApi = CThostFtdcTraderApi::CreateFtdcTraderApi(_flowPath.c_str());
  _tdApi->RegisterSpi(this);
  _tdApi->RegisterFront(const_cast<char *>(_frontAddr.c_str()));
  _tdApi->SubscribePrivateTopic(THOST_TERT_QUICK);
  _tdApi->SubscribePublicTopic(THOST_TERT_QUICK);
  _tdApi->Init();
  int ret = _tdApi->Join();
  SPDLOG_INFO("join returned, ret={}", ret);
}

void TradeCtp::disconnect() {
  SPDLOG_ERROR("disconnect");
  start();
}

void TradeCtp::OnFrontConnected() {
  if (int ret = login(); ret != 0) {
    SPDLOG_ERROR("login failed, ret={}", ret);
    return;
  }
}
void TradeCtp::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                              CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                              bool bIsLast) {
  SPDLOG_INFO("code={} msg={}", pRspInfo->ErrorID,
              ::EncodeUtf8("GBK", std::string(pRspInfo->ErrorMsg)));

  _frontId = pRspUserLogin->FrontID;
  _sessionId = pRspUserLogin->SessionID;
  _maxOrderRef = atoi(pRspUserLogin->MaxOrderRef);
  _tradingDay = atoi(_tdApi->GetTradingDay());

  // ReqQryInvestorPosition();
  // ReqQrySettlementInfo();
}
void TradeCtp::OnRspSettlementInfoConfirm(
    CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  SPDLOG_INFO("code={}", pRspInfo->ErrorID);
}
void TradeCtp::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument,
                                  CThostFtdcRspInfoField *pRspInfo,
                                  int nRequestID, bool bIsLast) {
  SPDLOG_INFO("onrspinstrument {0} ", bIsLast);
  // SPDLOG_INFO("onrspinstrument {0} ", pRspInfo->ErrorID);
  SPDLOG_INFO("{0} {1} {2} {3}", pInstrument->InstrumentID,
              pInstrument->InstrumentName, pInstrument->ExchangeID,
              pInstrument->UnderlyingInstrID);
}

void TradeCtp::OnRspQryTradingAccount(
    CThostFtdcTradingAccountField *pTradingAccount,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}

void TradeCtp::OnRspQryInvestorPosition(
    CThostFtdcInvestorPositionField *pInvestorPosition,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  SPDLOG_INFO("OnRspQryInvestorPosition, last={0}", bIsLast);
  SPDLOG_INFO(pRspInfo == nullptr);
  SPDLOG_INFO(pInvestorPosition->PosiDirection);

  // using json = nlohmann::json;
  // json j = json::parse(pInvestorPosition);
  // SPDLOG_INFO("{0} {1}", len);
}
void TradeCtp::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder,
                                CThostFtdcRspInfoField *pRspInfo,
                                int nRequestID, bool bIsLast) {}
void TradeCtp::OnRspOrderAction(
    CThostFtdcInputOrderActionField *pInputOrderAction,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}
void TradeCtp::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                          bool bIsLast) {}
void TradeCtp::OnFrontDisconnected(int nReason) { SPDLOG_INFO("disconnect"); }
void TradeCtp::OnHeartBeatWarning(int nTimeLapse) {}
void TradeCtp::OnRtnOrder(CThostFtdcOrderField *pOrder) {}
void TradeCtp::OnRtnTrade(CThostFtdcTradeField *pTrade) {}

TradeCtp::TradeCtp(std::string config_path) : ITrade(config_path) { init(); }

TradeCtp::~TradeCtp() { _tdApi->Release(); }

int TradeCtp::login() {
  CThostFtdcReqUserLoginField req = {0};
  strcpy(req.BrokerID, _brokerId.c_str());
  strcpy(req.UserID, _investorId.c_str());
  strcpy(req.Password, _password.c_str());

  return _tdApi->ReqUserLogin(&req, ++_requestId);
}

void TradeCtp::ReqQrySettlementInfo() {
  SPDLOG_INFO("ReqQrySettlementInfo");

  CThostFtdcQrySettlementInfoField req = {0};
  strcpy(req.BrokerID, _brokerId.c_str());
  strcpy(req.InvestorID, _investorId.c_str());
  SPDLOG_INFO("before settlement req");

  _tdApi->ReqQrySettlementInfo(&req, ++_requestId);
}

void TradeCtp::ReqQryInvestorPosition() {
  SPDLOG_INFO("ReqQryInvestorPosition");

  CThostFtdcQryInvestorPositionField req = {0};
  strcpy(req.BrokerID, _brokerId.c_str());
  strcpy(req.InvestorID, _investorId.c_str());

  SPDLOG_INFO("before position req");

  int ret = _tdApi->ReqQryInvestorPosition(&req, ++_requestId);
  SPDLOG_INFO("ret={0}", ret);
}

void TradeCtp::ReqQryInstrument() {
  CThostFtdcQryInstrumentField req = {0};
  _tdApi->ReqQryInstrument(&req, ++_requestId);
}

}  // namespace wss
