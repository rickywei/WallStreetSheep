#include "trade.hpp"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <iostream>
#include <nlohmann/json.hpp>
#include <unordered_map>

#include "codec.hpp"
#include "thread.hpp"

namespace wss {

constexpr std::string_view kTradingDay = "ctpTradingDay";
constexpr std::string_view kFmtRequestId = "ctpRequestId:{}";
constexpr std::string_view kFmtConfirmed = "ctpConfirmed:{}";
constexpr std::string_view chCtpHeartbeat = "chCtpHeartbeat";
constexpr std::string_view chCtpInstrument = "chCtpInstrument";
constexpr std::string_view chCtpOrder = "chCtpOrder";
constexpr std::string_view chCtpCancel = "chCtpCancel";
constexpr std::string_view chCtpOnRspOrderInsert = "chCtpOnRspOrderInsert";
constexpr std::string_view chCtpOnErrOrderInsert = "chCtpOnErrOrderInsert";
constexpr std::string_view chCtpOnRtnOrder = "chCtpOnRtnOrder";
constexpr std::string_view chCtpOnRtnTrade = "chCtpOnRtnTrade";
constexpr std::string_view chCtpOnRspOrderAction = "chCtpOnRspOrderAction";
constexpr std::string_view chCtpOnErrOrderAction = "chCtpOnErrOrderAction";

Trade::Trade(std::string str) {
  auto j = nlohmann::json::parse(str);
  j.at("tradeFrontAddr").get_to(_frontAddr);
  j.at("brokerId").get_to(_brokerId);
  j.at("investorId").get_to(_investorId);
  j.at("password").get_to(_password);
  j.at("redisAddr").get_to(_redisAddr);
  j.at("flowPath").get_to(_flowPath);
  _rc = std::make_unique<sw::redis::Redis>(_redisAddr);
}

Trade::~Trade() { _tdApi->Release(); }

void Trade::start() {
  SPDLOG_INFO("version={}", _tdApi->GetApiVersion());
  _tdApi = CThostFtdcTraderApi::CreateFtdcTraderApi(_flowPath.c_str());
  _tdApi->RegisterSpi(this);
  _tdApi->RegisterFront(const_cast<char *>(_frontAddr.c_str()));
  _tdApi->SubscribePrivateTopic(THOST_TERT_QUICK);
  _tdApi->SubscribePublicTopic(THOST_TERT_QUICK);
  _tdApi->Init();
  _daemon();
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

void Trade::OnFrontConnected() { _login(); }

void Trade::OnFrontDisconnected(int nReason) {
  SPDLOG_INFO("nReason={}", nReason);
  _isLogin = false;
  start();
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

  _rc->setex(kTradingDay, std::chrono::days(1), _tdApi->GetTradingDay());

  if (auto ret = _rc->get(fmt::format(kFmtConfirmed, _tdApi->GetTradingDay()));
      !ret.has_value()) {
    _querySettlementInfo();
  }
}

void Trade::OnRspQrySettlementInfo(
    CThostFtdcSettlementInfoField *pSettlementInfo,
    CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) {
  static std::string content;
  if (pRspInfo != nullptr && pRspInfo->ErrorID != 0) {
    SPDLOG_ERROR("nRequestID={}, code={}, msg={} ", nRequestID,
                 pRspInfo->ErrorID,
                 EncodeUtf8("GBK", std::string(pRspInfo->ErrorMsg)));
    content = "";
    return;
  }
  if (pSettlementInfo != nullptr) content += pSettlementInfo->Content;
  if (bIsLast) {
    SPDLOG_INFO("\n{}", EncodeUtf8("GBK", std::move(content)));
    _confirmSettlementInfo();
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
  if (bIsLast) {
    SPDLOG_INFO("confirmed");
    _rc->setex(fmt::format(kFmtConfirmed, _tdApi->GetTradingDay()),
               std::chrono::days(1), "1");
    _queryInstrument();
  }
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
  nlohmann::json j{
      {"ExchangeID", EncodeUtf8("GBK", std::string(pInstrument->ExchangeID))},
      {"InstrumentName",
       EncodeUtf8("GBK", std::string(pInstrument->InstrumentName))},
      {"ProductClass", std::string(1, pInstrument->ProductClass)},
      {"DeliveryYear", pInstrument->DeliveryYear},
      {"DeliveryMonth", pInstrument->DeliveryMonth},
      {"MaxMarketOrderVolume", pInstrument->MaxMarketOrderVolume},
      {"MinMarketOrderVolume", pInstrument->MinMarketOrderVolume},
      {"MaxLimitOrderVolume", pInstrument->MaxLimitOrderVolume},
      {"MinLimitOrderVolume", pInstrument->MinLimitOrderVolume},
      {"VolumeMultiple", pInstrument->VolumeMultiple},
      {"PriceTick", pInstrument->PriceTick},
      {"CreateDate", pInstrument->CreateDate},
      {"OpenDate", pInstrument->OpenDate},
      {"ExpireDate", pInstrument->ExpireDate},
      {"StartDelivDate", pInstrument->StartDelivDate},
      {"EndDelivDate", pInstrument->EndDelivDate},
      {"InstLifePhase", std::string(1, pInstrument->InstLifePhase)},
      {"IsTrading", pInstrument->IsTrading},
      {"PositionType", std::string(1, pInstrument->PositionType)},
      {"PositionDateType", std::string(1, pInstrument->PositionDateType)},
      {"LongMarginRatio", pInstrument->LongMarginRatio},
      {"ShortMarginRatio", pInstrument->ShortMarginRatio},
      {"MaxMarginSideAlgorithm",
       std::string(1, pInstrument->MaxMarginSideAlgorithm)},
      {"StrikePrice", pInstrument->StrikePrice},
      {"OptionsType", std::string(1, pInstrument->InstLifePhase)},
      {"UnderlyingMultiple", pInstrument->UnderlyingMultiple},
      {"CombinationType", std::string(1, pInstrument->InstLifePhase)},
      {"InstrumentID", pInstrument->InstrumentID},
      {"ExchangeInstID", pInstrument->ExchangeInstID},
      {"ProductID", pInstrument->ProductID},
      {"UnderlyingInstrID", pInstrument->UnderlyingInstrID},
  };
  _rc->publish(chCtpInstrument, j.dump());
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
    auto msg = EncodeUtf8("GBK", std::string(pRspInfo->ErrorMsg));
    SPDLOG_ERROR("nRequestID={}, code={}, msg={} ", nRequestID,
                 pRspInfo->ErrorID, msg);
    nlohmann::json j;
    j["TradingDay"] = _tdApi->GetTradingDay();
    j["RequestId"] = nRequestID;
    j["Error"] = msg;
    _rc->publish(chCtpOnRspOrderInsert, j.dump());
    return;
  }
}

void Trade::OnRtnOrder(CThostFtdcOrderField *pOrder) {
  std::string status =
      fmt::format("{}:{}", pOrder->OrderStatus,
                  EncodeUtf8("GBK", std::string(pOrder->StatusMsg)));
  SPDLOG_INFO(
      "RequestID={}, ExchangeID={}, InstrumentID={}, "
      "OrderSysID={}, "
      "status={}",
      pOrder->RequestID, pOrder->ExchangeID, pOrder->InstrumentID,
      pOrder->OrderSysID, status);
  nlohmann::json j;
  j["TradingDay"] = pOrder->TradingDay;
  j["RequestId"] = pOrder->RequestID;
  j["OrderSysID"] = pOrder->OrderSysID;
  j["Status"] = status;
  _rc->publish(chCtpOnRtnOrder, j.dump());
}

void Trade::OnRtnTrade(CThostFtdcTradeField *pTrade) {
  SPDLOG_INFO(
      "ExchangeID={}, InstrumentID={}, OrderSysID={}, TradeID={}, Price={}, "
      "Volume={}",
      pTrade->ExchangeID, pTrade->InstrumentID, pTrade->OrderSysID,
      pTrade->TradeID, pTrade->Price, pTrade->Volume);
  nlohmann::json j;
  j["TradingDay"] = pTrade->TradingDay;
  j["ExchangeID"] = pTrade->ExchangeID;
  j["InstrumentID"] = pTrade->InstrumentID;
  j["OrderSysID"] = pTrade->OrderSysID;
  j["TradeID"] = pTrade->TradeID;
  j["Price"] = pTrade->Price;
  j["Volume"] = pTrade->Volume;
  _rc->publish(chCtpOnRtnTrade, j.dump());
}

void Trade::OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder,
                                CThostFtdcRspInfoField *pRspInfo) {
  if (pRspInfo != nullptr && pRspInfo->ErrorID != 0) {
    auto msg = EncodeUtf8("GBK", std::string(pRspInfo->ErrorMsg));
    SPDLOG_ERROR("code={}, msg={} ", pRspInfo->ErrorID, msg);
    nlohmann::json j;
    j["TradingDay"] = _tdApi->GetTradingDay();
    j["RequestId"] = pInputOrder->RequestID;
    j["Error"] = msg;
    _rc->publish(chCtpOnErrOrderInsert, j.dump());
    return;
  }
}

void Trade::OnRspOrderAction(CThostFtdcInputOrderActionField *pInputOrderAction,
                             CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                             bool bIsLast) {
  if (pRspInfo != nullptr && pRspInfo->ErrorID != 0) {
    auto msg = EncodeUtf8("GBK", std::string(pRspInfo->ErrorMsg));
    SPDLOG_ERROR("nRequestID={}, code={}, msg={} ", nRequestID,
                 pRspInfo->ErrorID, msg);
    nlohmann::json j;
    j["ExchangeId"] = pInputOrderAction->ExchangeID;
    j["InstrumentId"] = pInputOrderAction->InstrumentID;
    j["OrderSysId"] = pInputOrderAction->OrderSysID;
    j["Error"] = msg;
    _rc->publish(chCtpOnRspOrderAction, j.dump());
    return;
  }
}

void Trade::OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction,
                                CThostFtdcRspInfoField *pRspInfo) {
  if (pRspInfo != nullptr && pRspInfo->ErrorID != 0) {
    auto msg = EncodeUtf8("GBK", std::string(pRspInfo->ErrorMsg));
    SPDLOG_ERROR("code={}, msg={}", pRspInfo->ErrorID, msg);
    nlohmann::json j;
    j["ExchangeId"] = pOrderAction->ExchangeID;
    j["InstrumentId"] = pOrderAction->InstrumentID;
    j["OrderSysId"] = pOrderAction->OrderSysID;
    j["Error"] = msg;
    _rc->publish(chCtpOnErrOrderAction, j.dump());
    return;
  }
}

void Trade::_daemon() {
  // heartbeat
  postTask([this]() {
    while (true) {
      if (this->_isLogin.load()) {
        _rc->publish(chCtpHeartbeat, "td");
      }
      std::this_thread::sleep_for(std::chrono::seconds(1));
    }
  });

  postTask([this]() {
    auto ch = this->_rc->subscriber();
    ch.subscribe(chCtpOrder);
    ch.subscribe(chCtpCancel);
    ch.on_message([this](std::string channel, std::string msg) {
      if (channel == chCtpOrder)
        this->_order(msg);
      else if (channel == chCtpCancel)
        this->_cancel(msg);
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

int Trade::_getRequestId() {
  return _rc->incr(fmt::format(kFmtRequestId, _tdApi->GetTradingDay()));
}

void Trade::_login() {
  CThostFtdcReqUserLoginField req = {0};
  strcpy(req.BrokerID, _brokerId.c_str());
  strcpy(req.UserID, _investorId.c_str());
  strcpy(req.Password, _password.c_str());
  if (int ret = _tdApi->ReqUserLogin(&req, _getRequestId()); ret != 0) {
    SPDLOG_ERROR("ret={}", ret);
    return;
  }
  _isLogin = true;
}

void Trade::_querySettlementInfo() {
  CThostFtdcQrySettlementInfoField req = {0};
  strcpy(req.BrokerID, _brokerId.c_str());
  strcpy(req.InvestorID, _investorId.c_str());
  if (int ret = _tdApi->ReqQrySettlementInfo(&req, _getRequestId()); ret != 0) {
    SPDLOG_ERROR("ret={}", ret);
  }
}

void Trade::_queryInstrument() {
  CThostFtdcQryInstrumentField req = {0};
  if (int ret = _tdApi->ReqQryInstrument(&req, _getRequestId()); ret != 0) {
    SPDLOG_ERROR("ret={}", ret);
  }
}

void Trade::_queryInvestorPosition() {
  CThostFtdcQryInvestorPositionField req = {0};
  strcpy(req.BrokerID, _brokerId.c_str());
  strcpy(req.InvestorID, _investorId.c_str());
  if (int ret = _tdApi->ReqQryInvestorPosition(&req, _getRequestId());
      ret != 0) {
    SPDLOG_ERROR("ret={}", ret);
  }
}

void Trade::_confirmSettlementInfo() {
  CThostFtdcSettlementInfoConfirmField req = {0};
  strcpy(req.BrokerID, _brokerId.c_str());
  strcpy(req.InvestorID, _investorId.c_str());
  if (int ret = _tdApi->ReqSettlementInfoConfirm(&req, _getRequestId());
      ret != 0) {
    SPDLOG_ERROR("ret={}", ret);
  }
}

void Trade::_order(std::string str) {
  SPDLOG_INFO("{}", str);
  auto req = CThostFtdcInputOrderField{
      .ContingentCondition = THOST_FTDC_CC_Immediately,
      .ForceCloseReason = THOST_FTDC_FCC_NotForceClose,
  };
  req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
  strcpy(req.BrokerID, _brokerId.c_str());
  strcpy(req.InvestorID, _investorId.c_str());
  int requestId = 0;
  try {
    auto j = nlohmann::json::parse(str);
    j.at("RequestId").get_to(requestId);
    if (requestId == 0) {
      throw "zero request id";
    }
    req.RequestID = requestId;
    strcpy(req.ExchangeID, j.at("ExchangeId").get<std::string>().c_str());
    strcpy(req.InstrumentID, j.at("InstrumentId").get<std::string>().c_str());
    req.Direction = j.at("Direction").get<std::string>()[0];
    req.CombOffsetFlag[0] = j.at("Offset").get<std::string>().c_str()[0];
    j.at("Volume").get_to(req.VolumeTotalOriginal);
    j.at("Price").get_to(req.LimitPrice);
    if (req.LimitPrice == 0) {
      req.OrderPriceType = THOST_FTDC_OPT_AnyPrice;
      req.TimeCondition = THOST_FTDC_TC_IOC;
      req.VolumeCondition = THOST_FTDC_VC_AV;
    } else {
      req.OrderPriceType = THOST_FTDC_OPT_LimitPrice;
      req.TimeCondition = THOST_FTDC_TC_GFD;
      if (std::string mode = j.at("Mode").get<std::string>(); mode == "FAK") {
        req.TimeCondition = THOST_FTDC_TC_IOC;
        req.VolumeCondition = THOST_FTDC_VC_AV;
      } else if (mode == "FOK") {
        req.TimeCondition = THOST_FTDC_TC_IOC;
        req.VolumeCondition = THOST_FTDC_VC_CV;
      }
    }

  } catch (const std::exception &e) {
    SPDLOG_ERROR("{}", e.what());
    return;
  }
  if (int ret = _tdApi->ReqOrderInsert(&req, requestId); ret != 0) {
    SPDLOG_ERROR("ret={}", ret);
  }
}

void Trade::_cancel(std::string str) {
  auto req = CThostFtdcInputOrderActionField{
      .ActionFlag = THOST_FTDC_AF_Delete,
  };
  strcpy(req.BrokerID, _brokerId.c_str());
  strcpy(req.InvestorID, _investorId.c_str());
  try {
    auto j = nlohmann::json::parse(str);
    j.at("ExchangeId").get_to(req.ExchangeID);
    j.at("InstrumentId").get_to(req.InstrumentID);
    j.at("OrderSysId").get_to(req.OrderSysID);
  } catch (const std::exception &e) {
    SPDLOG_ERROR("{}", e.what());
    return;
  }
  if (int ret = _tdApi->ReqOrderAction(&req, _getRequestId()); ret != 0) {
    SPDLOG_ERROR("ret={}", ret);
  }
}

}  // namespace wss
