#include "WallStreetSheep/trade/TradeCtp.hpp"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <iostream>
#include <nlohmann/json.hpp>
#include <unordered_set>

#include "WallStreetSheep/common/common.hpp"
#include "WallStreetSheep/convert/codec.hpp"

namespace wss {

TradeCtp::TradeCtp(std::string config_path) : ITrade(config_path) { init(); }

TradeCtp::~TradeCtp() { _tdApi->Release(); }

void TradeCtp::init() {
  YAML::Node config = YAML::LoadFile(_configPath);
  YAML::Node trade_config = config["trade"];
  _frontAddr = trade_config["front_addr"].as<std::string>();
  _brokerId = trade_config["broker_id"].as<std::string>();
  _investorId = trade_config["investor_id"].as<std::string>();
  _password = trade_config["password"].as<std::string>();
  _flowPath = trade_config["flow_path"].as<std::string>();
}

void TradeCtp::start() {
  SPDLOG_INFO("version={}", _tdApi->GetApiVersion());
  _tdApi = CThostFtdcTraderApi::CreateFtdcTraderApi(_flowPath.c_str());
  _tdApi->RegisterSpi(this);
  _tdApi->RegisterFront(const_cast<char *>(_frontAddr.c_str()));
  _tdApi->SubscribePrivateTopic(THOST_TERT_QUICK);
  _tdApi->SubscribePublicTopic(THOST_TERT_QUICK);
  _tdApi->Init();
  _logged.wait(false);
  SPDLOG_INFO("inited");
  _inited.store(true, std::memory_order_release);
  _inited.notify_one();
  int ret = _tdApi->Join();
  SPDLOG_INFO("join returned, ret={}", ret);
}

void TradeCtp::disconnect() {
  SPDLOG_ERROR("");
  _inited.store(false, std::memory_order_release);
  start();
}

void TradeCtp::OnFrontConnected() {
  if (int ret = login(); ret != 0) {
    SPDLOG_ERROR("ret={}", ret);
  }
}
void TradeCtp::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                              CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                              bool bIsLast) {
  if (pRspInfo != nullptr && pRspInfo->ErrorID != 0) {
    SPDLOG_ERROR("code={}, msg={}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    return;
  }
  _frontId = pRspUserLogin->FrontID;
  _sessionId = pRspUserLogin->SessionID;
  _maxOrderRef = atoi(pRspUserLogin->MaxOrderRef);
  _tradingDay = atoi(_tdApi->GetTradingDay());

  _logged.store(true, std::memory_order_release);
  _logged.notify_one();

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
  if (pRspInfo != nullptr && pRspInfo->ErrorID != 0) {
    SPDLOG_ERROR("nRequestID={}, code={}, msg={} ", nRequestID,
                 pRspInfo->ErrorID,
                 EncodeUtf8("GBK", std::string(pRspInfo->ErrorMsg)));
    return;
  }
  auto sp = std::make_shared<CThostFtdcInstrumentField>(*pInstrument);
  strcpy(sp->ExchangeID, pInstrument->ExchangeID);
  strcpy(sp->InstrumentName, pInstrument->InstrumentName);
  strcpy(sp->CreateDate, pInstrument->CreateDate);
  strcpy(sp->OpenDate, pInstrument->OpenDate);
  strcpy(sp->ExpireDate, pInstrument->ExpireDate);
  strcpy(sp->StartDelivDate, pInstrument->StartDelivDate);
  strcpy(sp->EndDelivDate, pInstrument->EndDelivDate);
  strcpy(sp->InstrumentID, pInstrument->InstrumentID);
  strcpy(sp->ExchangeInstID, pInstrument->ExchangeInstID);
  strcpy(sp->ProductID, pInstrument->ProductID);
  strcpy(sp->UnderlyingInstrID, pInstrument->UnderlyingInstrID);
  _instruments[pInstrument->InstrumentID] = sp;
  
  if (bIsLast) {
    SPDLOG_INFO("nRequestID={}, total={}", nRequestID, _instruments.size());
  }

  _db->write(
      R"(INSERT INTO
        InstrumentCtp(
            ExchangeID, InstrumentName, ProductClass, DeliveryYear,
            DeliveryMonth, MaxMarketOrderVolume, MinMarketOrderVolume,
            MaxLimitOrderVolume, MinLimitOrderVolume, VolumeMultiple, PriceTick,
            CreateDate, OpenDate, ExpireDate, StartDelivDate, EndDelivDate,
            InstLifePhase, IsTrading, PositionType, PositionDateType,
            LongMarginRatio, ShortMarginRatio, MaxMarginSideAlgorithm,
            StrikePrice, OptionsType, UnderlyingMultiple, CombinationType,
            InstrumentID, ExchangeInstID, ProductID, UnderlyingInstrID)
            VALUES($1, $2, $3, $4, $5, $6, $7, $8, $9, $10, $11, $12, $13, $14,
                   $15, $16, $17, $18, $19, $20, $21, $22, $23, $24, $25, $26,
                   $27, $28, $29, $30, $31) ON CONFLICT DO NOTHING)",
      sp->ExchangeID, EncodeUtf8("GBK", std::string(sp->InstrumentName)),
      std::string(1, sp->ProductClass), sp->DeliveryYear, sp->DeliveryMonth,
      sp->MaxMarketOrderVolume, sp->MinMarketOrderVolume,
      sp->MaxLimitOrderVolume, sp->MinLimitOrderVolume, sp->VolumeMultiple,
      sp->PriceTick, sp->CreateDate, sp->OpenDate, sp->ExpireDate,
      sp->StartDelivDate, sp->EndDelivDate, std::string(1, sp->InstLifePhase),
      sp->IsTrading, std::string(1, sp->PositionType),
      std::string(1, sp->PositionDateType), sp->LongMarginRatio,
      sp->ShortMarginRatio, std::string(1, sp->MaxMarginSideAlgorithm),
      sp->StrikePrice, std::string(1, sp->OptionsType), sp->UnderlyingMultiple,
      std::string(1, sp->CombinationType), sp->InstrumentID, sp->ExchangeInstID,
      sp->ProductID, sp->UnderlyingInstrID);
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
