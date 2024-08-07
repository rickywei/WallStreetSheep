#include "ctp/Trade.hpp"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <iostream>
#include <nlohmann/json.hpp>
#include <unordered_set>

#include "common/codec.hpp"
#include "common/thread.hpp"
#include "ctp/utils.hpp"

namespace wss {

Trade::Trade(std::string config_path) : ICtp(config_path) { init(); }

Trade::~Trade() { _tdApi->Release(); }

void Trade::init() {
  YAML::Node config = YAML::LoadFile(_configPath);
  YAML::Node trade_config = config["trade"];
  _frontAddr = trade_config["front_addr"].as<std::string>();
  _brokerId = trade_config["broker_id"].as<std::string>();
  _investorId = trade_config["investor_id"].as<std::string>();
  _password = trade_config["password"].as<std::string>();
  _flowPath = trade_config["flow_path"].as<std::string>();
}

void Trade::start() {
  SPDLOG_INFO("version={}", _tdApi->GetApiVersion());
  _tdApi = CThostFtdcTraderApi::CreateFtdcTraderApi(_flowPath.c_str());
  _tdApi->RegisterSpi(this);
  _tdApi->RegisterFront(const_cast<char *>(_frontAddr.c_str()));
  _tdApi->SubscribePrivateTopic(THOST_TERT_QUICK);
  _tdApi->SubscribePublicTopic(THOST_TERT_QUICK);
  _tdApi->Init();
  _started.wait(false);
  SPDLOG_INFO("inited");
  postTask([this]() {
    int ret = this->_tdApi->Join();
    SPDLOG_INFO("join returned, ret={}", ret);
  });
}

void Trade::OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                       bool bIsLast) {
  if (pRspInfo != nullptr && pRspInfo->ErrorID != 0) {
    SPDLOG_ERROR("code={}, msg={}", pRspInfo->ErrorID, pRspInfo->ErrorMsg);
    return;
  }
}

void Trade::OnFrontConnected() {
  if (int ret = login(); ret != 0) {
    SPDLOG_ERROR("ret={}", ret);
  }
}

void Trade::OnFrontDisconnected(int nReason) {
  SPDLOG_INFO("nReason={}", nReason);
}

void Trade::OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                           CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                           bool bIsLast) {
  if (pRspInfo != nullptr && pRspInfo->ErrorID != 0) {
    SPDLOG_ERROR("code={}, msg={}", pRspInfo->ErrorID,
                 EncodeUtf8("GBK", std::string(pRspInfo->ErrorMsg)));
    return;
  }
  _frontId = pRspUserLogin->FrontID;
  _sessionId = pRspUserLogin->SessionID;
  _maxOrderRef = atoi(pRspUserLogin->MaxOrderRef);
  _tradingDay = atoi(_tdApi->GetTradingDay());

  querySettlementInfo();
}

void Trade::OnRspQrySettlementInfo(
    CThostFtdcSettlementInfoField *pSettlementInfo,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  static std::unordered_map<int, std::string> id2content;
  static std::mutex mtx;
  std::unique_lock lk(mtx);
  if (pRspInfo != nullptr && pRspInfo->ErrorID != 0) {
    SPDLOG_ERROR("nRequestID={}, code={}, msg={} ", nRequestID,
                 pRspInfo->ErrorID,
                 EncodeUtf8("GBK", std::string(pRspInfo->ErrorMsg)));
    id2content.erase(nRequestID);
    return;
  }
  if (bIsLast) {
    SPDLOG_INFO("\n{}", EncodeUtf8("GBK", std::move(id2content[nRequestID])));
    id2content.erase(nRequestID);

    confirmSettlementInfo();
  }
}

void Trade::OnRspSettlementInfoConfirm(
    CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  if (pRspInfo != nullptr && pRspInfo->ErrorID != 0) {
    SPDLOG_ERROR("nRequestID={}, code={}, msg={} ", nRequestID,
                 pRspInfo->ErrorID,
                 EncodeUtf8("GBK", std::string(pRspInfo->ErrorMsg)));
    return;
  }

  queryInstrument();
}

void Trade::OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument,
                               CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                               bool bIsLast) {
  if (pRspInfo != nullptr && pRspInfo->ErrorID != 0) {
    SPDLOG_ERROR("nRequestID={}, code={}, msg={} ", nRequestID,
                 pRspInfo->ErrorID,
                 EncodeUtf8("GBK", std::string(pRspInfo->ErrorMsg)));
    return;
  }
  auto sp = toInstrumentData(pInstrument);
  instruments[pInstrument->InstrumentID] = sp;
  if (bIsLast) {
    SPDLOG_INFO("nRequestID={}, total={}", nRequestID, instruments.size());

    _started.store(true, std::memory_order_release);
    _started.notify_one();
  }
  _db->write(
      R"(INSERT INTO
        Instrument(
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
      sp->ExchangeID, sp->InstrumentName, sp->ProductClass, sp->DeliveryYear,
      sp->DeliveryMonth, sp->MaxMarketOrderVolume, sp->MinMarketOrderVolume,
      sp->MaxLimitOrderVolume, sp->MinLimitOrderVolume, sp->VolumeMultiple,
      sp->PriceTick, sp->CreateDate, sp->OpenDate, sp->ExpireDate,
      sp->StartDelivDate, sp->EndDelivDate, sp->InstLifePhase, sp->IsTrading,
      sp->PositionType, sp->PositionDateType, sp->LongMarginRatio,
      sp->ShortMarginRatio, sp->MaxMarginSideAlgorithm, sp->StrikePrice,
      sp->OptionsType, sp->UnderlyingMultiple, sp->CombinationType,
      sp->InstrumentID, sp->ExchangeInstID, sp->ProductID,
      sp->UnderlyingInstrID);
}

void Trade::OnRspQryInvestorPosition(
    CThostFtdcInvestorPositionField *pInvestorPosition,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  if (pRspInfo != nullptr && pRspInfo->ErrorID != 0) {
    SPDLOG_ERROR("nRequestID={}, code={}, msg={} ", nRequestID,
                 pRspInfo->ErrorID,
                 EncodeUtf8("GBK", std::string(pRspInfo->ErrorMsg)));
    return;
  }
}

void Trade::OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder,
                             CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                             bool bIsLast) {
  if (pRspInfo != nullptr && pRspInfo->ErrorID != 0) {
    SPDLOG_ERROR("nRequestID={}, code={}, msg={} ", nRequestID,
                 pRspInfo->ErrorID,
                 EncodeUtf8("GBK", std::string(pRspInfo->ErrorMsg)));
    return;
  }
}

void Trade::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction,
                             CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                             bool bIsLast) {
  if (pRspInfo != nullptr && pRspInfo->ErrorID != 0) {
    SPDLOG_ERROR("nRequestID={}, code={}, msg={} ", nRequestID,
                 pRspInfo->ErrorID,
                 EncodeUtf8("GBK", std::string(pRspInfo->ErrorMsg)));
    return;
  }
}

void Trade::OnRtnOrder(CThostFtdcOrderField *pOrder) {
  SPDLOG_INFO("{}", pOrder->OrderSysID);
  // TODO callback
}

void Trade::OnRtnTrade(CThostFtdcTradeField *pTrade) {
  SPDLOG_INFO("{}", pTrade->OrderSysID);
  // TODO callback
}

void Trade::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder,
                                CThostFtdcRspInfoField *pRspInfo) {
  if (pRspInfo != nullptr && pRspInfo->ErrorID != 0) {
    SPDLOG_ERROR("code={}, msg={} ", pRspInfo->ErrorID,
                 EncodeUtf8("GBK", std::string(pRspInfo->ErrorMsg)));
    return;
  }
  SPDLOG_INFO("{}", pInputOrder->OrderRef);
}

void Trade::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction,
                                CThostFtdcRspInfoField *pRspInfo) {
  if (pRspInfo != nullptr && pRspInfo->ErrorID != 0) {
    SPDLOG_ERROR("code={}, msg={} ", pRspInfo->ErrorID,
                 EncodeUtf8("GBK", std::string(pRspInfo->ErrorMsg)));
    return;
  }
  SPDLOG_ERROR("{}", pOrderAction->OrderSysID);
}

int Trade::login() {
  CThostFtdcReqUserLoginField req = {0};
  strcpy(req.BrokerID, _brokerId.c_str());
  strcpy(req.UserID, _investorId.c_str());
  strcpy(req.Password, _password.c_str());

  return _tdApi->ReqUserLogin(&req, ++_requestId);
}

int Trade::querySettlementInfo() {
  CThostFtdcQrySettlementInfoField req = {0};
  strcpy(req.BrokerID, _brokerId.c_str());
  strcpy(req.InvestorID, _investorId.c_str());
  int ret = _tdApi->ReqQrySettlementInfo(&req, ++_requestId);
  if (ret != 0) SPDLOG_ERROR("ret={}", ret);
  return ret;
}

int Trade::queryInvestorPosition() {
  CThostFtdcQryInvestorPositionField req = {0};
  strcpy(req.BrokerID, _brokerId.c_str());
  strcpy(req.InvestorID, _investorId.c_str());
  int ret = _tdApi->ReqQryInvestorPosition(&req, ++_requestId);
  if (ret != 0) SPDLOG_ERROR("ret={}", ret);
  return ret;
}

int Trade::queryInstrument() {
  CThostFtdcQryInstrumentField req = {0};
  int ret = _tdApi->ReqQryInstrument(&req, ++_requestId);
  if (ret != 0) SPDLOG_ERROR("ret={}", ret);
  return ret;
}

int Trade::confirmSettlementInfo() {
  CThostFtdcSettlementInfoConfirmField req = {0};
  strcpy(req.BrokerID, _brokerId.c_str());
  strcpy(req.InvestorID, _investorId.c_str());
  int ret = _tdApi->ReqSettlementInfoConfirm(&req, ++_requestId);
  if (ret != 0) SPDLOG_ERROR("ret={}", ret);
  return ret;
}

int Trade::order(std::string exchangeId, std::string instrumentId, int volume,
                 double limitPrice, double stopPrice,
                 TThostFtdcDirectionType direction,
                 TThostFtdcOffsetFlagType offset,
                 TThostFtdcOrderPriceTypeType priceType,
                 TThostFtdcContingentConditionType condition, Mode mode) {
  CThostFtdcInputOrderField req = {0};
  strcpy(req.BrokerID, _brokerId.c_str());
  strcpy(req.InvestorID, _investorId.c_str());
  strcpy(req.ExchangeID, exchangeId.c_str());
  strcpy(req.InstrumentID, instrumentId.c_str());
  strcpy(req.OrderRef, "12344");
  req.Direction = direction;
  req.CombOffsetFlag[0] = offset;
  req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
  req.VolumeTotalOriginal = volume;
  req.MinVolume = 1;
  req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
  req.OrderPriceType = priceType;
  if (mode == Mode::None) {
    req.TimeCondition = THOST_FTDC_TC_GFD;
    req.VolumeCondition = THOST_FTDC_VC_AV;
  } else if (mode == Mode::FAK) {
    req.TimeCondition = THOST_FTDC_TC_IOC;
    req.VolumeCondition = THOST_FTDC_VC_AV;
  } else if (mode == Mode::FOK) {
    req.TimeCondition = THOST_FTDC_TC_IOC;
    req.VolumeCondition = THOST_FTDC_VC_CV;
  }
  req.ContingentCondition = condition;
  if (stopPrice != 0) {
    req.StopPrice = stopPrice;
  }
  int ret = _tdApi->ReqOrderInsert(&req, ++_requestId);
  if (ret != 0) {
    SPDLOG_ERROR("ret={}", ret);
  }
  return ret;
}

int Trade::cancelOrder(std::string exchangeId, std::string instrumentId,
                       std::string orderSysId) {
  CThostFtdcInputOrderActionField req = {0};
  strcpy(req.BrokerID, _brokerId.c_str());
  strcpy(req.InvestorID, _investorId.c_str());
  strcpy(req.InstrumentID, instrumentId.c_str());
  strcpy(req.ExchangeID, exchangeId.c_str());
  strcpy(req.OrderSysID, orderSysId.c_str());
  req.ActionFlag = THOST_FTDC_AF_Delete;

  int ret = _tdApi->ReqOrderAction(&req, ++_requestId);
  if (ret != 0) {
    SPDLOG_ERROR("ret={}", ret);
  }
  return ret;
}

}  // namespace wss
