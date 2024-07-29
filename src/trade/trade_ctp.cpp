#include "alphamaker/trade/trade_ctp.hpp"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <iostream>
#include <nlohmann/json.hpp>

#include "convert/codec.hpp"

namespace am {
void TradeCtp::LoadConfig() {
  YAML::Node config = YAML::LoadFile(_config_path);
  YAML::Node trade_config = config["trade"];
  _front_addr = trade_config["front_addr"].as<std::string>();
  _broker_id = trade_config["broker_id"].as<std::string>();
  _investor_id = trade_config["investor_id"].as<std::string>();
  _password = trade_config["password"].as<std::string>();
  _flowPath = trade_config["flow_path"].as<std::string>();
  SPDLOG_INFO("read trade config...\n{0}", YAML::Dump(trade_config));
}

void TradeCtp::Init() {
  SPDLOG_INFO("version={0}", _td_api->GetApiVersion());
  _td_api = CThostFtdcTraderApi::CreateFtdcTraderApi(_flowPath.c_str());
  _td_api->RegisterSpi(this);
}

void TradeCtp::Connect() {
  SPDLOG_INFO("connect front");
  _td_api->RegisterFront(const_cast<char *>(_front_addr.c_str()));
  _td_api->SubscribePrivateTopic(THOST_TERT_QUICK);
  _td_api->SubscribePublicTopic(THOST_TERT_QUICK);
  _td_api->Init();
  _td_api->Join();
}

void TradeCtp::Disconnect() {
  SPDLOG_ERROR("disconnect");
  Connect();
}

void TradeCtp::OnFrontConnected() {
  if (int ret = Login(); ret != 0) {
    SPDLOG_ERROR("login failed...ret = {}", ret);
    return;
  }
}
void TradeCtp::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                              CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                              bool bIsLast) {
  SPDLOG_INFO("onRspUserLogin code={} msg={}", pRspInfo->ErrorID,
              ::EncodeUtf8("GBK", std::string(pRspInfo->ErrorMsg)));

  _front_id = pRspUserLogin->FrontID;
  _session_id = pRspUserLogin->SessionID;
  _max_order_ref = atoi(pRspUserLogin->MaxOrderRef);
  _trading_day = atoi(_td_api->GetTradingDay());

  // ReqQryInvestorPosition();
  // ReqQrySettlementInfo();
}
void TradeCtp::OnRspSettlementInfoConfirm(
    CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  SPDLOG_INFO("OnRspSettlementInfoConfirm code={0}", pRspInfo->ErrorID);
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

TradeCtp::TradeCtp(std::string config_path) : Trade(config_path) {
  LoadConfig();
}

TradeCtp::~TradeCtp() { _td_api->Release(); }

int TradeCtp::Login() {
  SPDLOG_INFO("login");

  CThostFtdcReqUserLoginField req;
  memset(&req, 0, sizeof(req));
  strcpy(req.BrokerID, _broker_id.c_str());
  strcpy(req.UserID, _investor_id.c_str());
  strcpy(req.Password, _password.c_str());

  return _td_api->ReqUserLogin(&req, ++_requestId);
}

void TradeCtp::ReqQrySettlementInfo() {
  SPDLOG_INFO("ReqQrySettlementInfo");

  CThostFtdcQrySettlementInfoField req;
  memset(&req, 0, sizeof(req));
  strcpy(req.BrokerID, _broker_id.c_str());
  strcpy(req.InvestorID, _investor_id.c_str());
  SPDLOG_INFO("before settlement req");

  _td_api->ReqQrySettlementInfo(&req, ++_requestId);
}

void TradeCtp::ReqQryInvestorPosition() {
  SPDLOG_INFO("ReqQryInvestorPosition");

  CThostFtdcQryInvestorPositionField req = {0};
  // memset(&req, 0, sizeof(req));
  strcpy(req.BrokerID, _broker_id.c_str());
  strcpy(req.InvestorID, _investor_id.c_str());

  SPDLOG_INFO("before position req");

  int ret = _td_api->ReqQryInvestorPosition(&req, ++_requestId);
  SPDLOG_INFO("ret={0}", ret);
}

void TradeCtp::ReqQryInstrument() {
  SPDLOG_INFO("qryinstrument");
  CThostFtdcQryInstrumentField req = {0};
  // memset(&req, 0, sizeof(req));
  _td_api->ReqQryInstrument(&req, ++_requestId);
}

}  // namespace am
