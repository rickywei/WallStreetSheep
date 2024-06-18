#include "alphamaker/trade/trade_ctp.hpp"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <iostream>
#include <nlohmann/json.hpp>

#include "alphamaker/trade/trade_ctp.hpp"
#include "convert/codec.hpp"

namespace am {
void TradeCtp::LoadConfig() {
  YAML::Node config = YAML::LoadFile(_config_path);
  YAML::Node trade_config = config["trade"];
  spdlog::info("read config...\n{0}", YAML::Dump(trade_config));
  _front_addr = trade_config["front_addr"].as<std::string>();
  _broker_id = trade_config["broker_id"].as<std::string>();
  _investor_id = trade_config["investor_id"].as<std::string>();
  _password = trade_config["password"].as<std::string>();
  _flowPath = trade_config["flow_path"].as<std::string>();
}

void TradeCtp::Init() {
  spdlog::info("version={0}", _td_api->GetApiVersion());
  _td_api = CThostFtdcTraderApi::CreateFtdcTraderApi(_flowPath.c_str());
  _td_api->RegisterSpi(this);
}

void TradeCtp::Connect() {
  _td_api->RegisterFront(const_cast<char *>(_front_addr.c_str()));
  _td_api->SubscribePrivateTopic(THOST_TERT_QUICK);
  _td_api->SubscribePublicTopic(THOST_TERT_QUICK);
  _td_api->Init();
  spdlog::info("after init");
  _td_api->Join();
  spdlog::info("connect finished");
}

void TradeCtp::Disconnect() {
  spdlog::error("disconnect");
  Connect();
}

void TradeCtp::OnFrontConnected() {
  if (int ret = Login(); ret != 0) {
    spdlog::error("login failed...ret = {}", ret);
    return;
  }
}
void TradeCtp::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                              CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                              bool bIsLast) {
  spdlog::info("onRspUserLogin code={} msg={}", pRspInfo->ErrorID,
               ::EncodeUtf8("GBK", std::string(pRspInfo->ErrorMsg)));

  _front_id = pRspUserLogin->FrontID;
  _session_id = pRspUserLogin->SessionID;
  _max_order_ref = atoi(pRspUserLogin->MaxOrderRef);
  _trading_day = atoi(_td_api->GetTradingDay());

  spdlog::info("_front_id={0}", _front_id);
  spdlog::info("_session_id={0}", _session_id);
  spdlog::info("_max_order_ref={0}", _max_order_ref);
  spdlog::info("_trading_day={0}", _trading_day);

  // ReqQryInvestorPosition();
}
void TradeCtp::OnRspSettlementInfoConfirm(
    CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}
void TradeCtp::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument,
                                  CThostFtdcRspInfoField *pRspInfo,
                                  int nRequestID, bool bIsLast) {}
void TradeCtp::OnRspQryTradingAccount(
    CThostFtdcTradingAccountField *pTradingAccount,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}
void TradeCtp::OnRspQryInvestorPosition(
    CThostFtdcInvestorPositionField *pInvestorPosition,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  spdlog::info("OnRspQryInvestorPosition code={0} msg={0}", pRspInfo->ErrorID,
                std::string(pRspInfo->ErrorMsg));

  // using json = nlohmann::json;
  // json j = json::parse(pInvestorPosition);
  // spdlog::info("{0} {1}", len);
}
void TradeCtp::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder,
                                CThostFtdcRspInfoField *pRspInfo,
                                int nRequestID, bool bIsLast) {}
void TradeCtp::OnRspOrderAction(
    CThostFtdcInputOrderActionField *pInputOrderAction,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {}
void TradeCtp::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                          bool bIsLast) {}
void TradeCtp::OnFrontDisconnected(int nReason) {
  spdlog::info("disconnect");
}
void TradeCtp::OnHeartBeatWarning(int nTimeLapse) {}
void TradeCtp::OnRtnOrder(CThostFtdcOrderField *pOrder) {}
void TradeCtp::OnRtnTrade(CThostFtdcTradeField *pTrade) {}

TradeCtp::TradeCtp(std::string config_path) : Trade(config_path) {
  LoadConfig();
}

TradeCtp::~TradeCtp() { _td_api->Release(); }

int TradeCtp::Login() {
  spdlog::info("login");

  CThostFtdcReqUserLoginField req;
  memset(&req, 0, sizeof(req));
  strcpy(req.BrokerID, _broker_id.c_str());
  strcpy(req.UserID, _investor_id.c_str());
  strcpy(req.Password, _password.c_str());

  spdlog::info("req.broker_id={0}", req.BrokerID);
  spdlog::info("req.userid={0}", req.UserID);
  spdlog::info("req.password={0}", req.Password);

  return _td_api->ReqUserLogin(&req, ++_requestId);
}

void TradeCtp::ReqQrySettlementInfo() {
  spdlog::info("ReqQrySettlementInfo");

  CThostFtdcQrySettlementInfoField req;
  memset(&req, 0, sizeof(req));
  strcpy(req.BrokerID, _broker_id.c_str());
  strcpy(req.InvestorID, _investor_id.c_str());

  _td_api->ReqQrySettlementInfo(&req, ++_requestId);
}

void TradeCtp::ReqQryInvestorPosition() {
  spdlog::info("ReqQryInvestorPosition");

  CThostFtdcQryInvestorPositionField req;
  memset(&req, 0, sizeof(req));
  strcpy(req.BrokerID, _broker_id.c_str());
  strcpy(req.InvestorID, _investor_id.c_str());

  spdlog::info("before position req");

  _td_api->ReqQryInvestorPosition(&req, ++_requestId);
}

}  // namespace am
