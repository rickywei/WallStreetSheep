#include "Trade.hpp"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <iostream>
#include <unordered_set>

#include "../../common/codec.hpp"
#include "../../common/thread.hpp"
#include "../encoding/json.hpp"

namespace wss {

constexpr std::string chInstrument = "ctpInstrument";
constexpr std::string kFmtConfirmed = "ctpConfirmed:{}";

Trade::Trade() {
  init();
  start();
}

Trade::~Trade() { _tdApi->Release(); }

void Trade::init() {
  YAML::Node conf = YAML::LoadFile("./conf.yaml");
  _frontAddr = conf["front_addr"].as<std::string>();
  _brokerId = conf["broker_id"].as<std::string>();
  _investorId = conf["investor_id"].as<std::string>();
  _password = conf["password"].as<std::string>();
  _flowPath = conf["flow_path"].as<std::string>();
  _rc = std::make_unique<sw::redis::Redis>(conf["redisAddr"].as<std::string>());
}

void Trade::start() {
  SPDLOG_INFO("version={}", _tdApi->GetApiVersion());
  _tdApi = CThostFtdcTraderApi::CreateFtdcTraderApi(_flowPath.c_str());
  _tdApi->RegisterSpi(this);
  _tdApi->RegisterFront(const_cast<char *>(_frontAddr.c_str()));
  _tdApi->SubscribePrivateTopic(THOST_TERT_QUICK);
  _tdApi->SubscribePublicTopic(THOST_TERT_QUICK);
  _tdApi->Init();
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

  if (auto ret = _rc->get(fmt::format(kFmtConfirmed, Date()));
      ret.has_value()) {
  } else {
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
  content += pSettlementInfo->Content;
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

  SPDLOG_INFO("confirmed");
  _rc->setex(fmt::format(kFmtConfirmed, Date()), std::chrono::days(1), "1");

  _queryInstrument();
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
  _rc->publish(chInstrument, marshal(pInstrument));
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

int Trade::_login() {
  CThostFtdcReqUserLoginField req = {0};
  strcpy(req.BrokerID, _brokerId.c_str());
  strcpy(req.UserID, _investorId.c_str());
  strcpy(req.Password, _password.c_str());
  int ret = _tdApi->ReqUserLogin(&req, ++_requestId);
  if (ret != 0) SPDLOG_ERROR("ret={}", ret);
  return ret;
}

int Trade::_querySettlementInfo() {
  CThostFtdcQrySettlementInfoField req = {0};
  strcpy(req.BrokerID, _brokerId.c_str());
  strcpy(req.InvestorID, _investorId.c_str());
  int ret = _tdApi->ReqQrySettlementInfo(&req, ++_requestId);
  if (ret != 0) SPDLOG_ERROR("ret={}", ret);
  return ret;
}

int Trade::_queryInvestorPosition() {
  CThostFtdcQryInvestorPositionField req = {0};
  strcpy(req.BrokerID, _brokerId.c_str());
  strcpy(req.InvestorID, _investorId.c_str());
  int ret = _tdApi->ReqQryInvestorPosition(&req, ++_requestId);
  if (ret != 0) SPDLOG_ERROR("ret={}", ret);
  return ret;
}

int Trade::_queryInstrument() {
  CThostFtdcQryInstrumentField req = {0};
  int ret = _tdApi->ReqQryInstrument(&req, ++_requestId);
  if (ret != 0) SPDLOG_ERROR("ret={}", ret);
  return ret;
}

int Trade::_confirmSettlementInfo() {
  CThostFtdcSettlementInfoConfirmField req = {0};
  strcpy(req.BrokerID, _brokerId.c_str());
  strcpy(req.InvestorID, _investorId.c_str());
  int ret = _tdApi->ReqSettlementInfoConfirm(&req, ++_requestId);
  if (ret != 0) SPDLOG_ERROR("ret={}", ret);
  return ret;
}

// int Trade::order(std::string exchangeId, std::string instrumentId, int
// volume,
//                  double limitPrice, double stopPrice,
//                  TThostFtdcDirectionType direction,
//                  TThostFtdcOffsetFlagType offset,
//                  TThostFtdcOrderPriceTypeType priceType,
//                  TThostFtdcContingentConditionType condition, Mode mode) {
//   CThostFtdcInputOrderField req = {0};
//   strcpy(req.BrokerID, _brokerId.c_str());
//   strcpy(req.InvestorID, _investorId.c_str());
//   strcpy(req.ExchangeID, exchangeId.c_str());
//   strcpy(req.InstrumentID, instrumentId.c_str());
//   strcpy(req.OrderRef, "12344");
//   req.Direction = direction;
//   req.CombOffsetFlag[0] = offset;
//   req.CombHedgeFlag[0] = THOST_FTDC_HF_Speculation;
//   req.VolumeTotalOriginal = volume;
//   req.MinVolume = 1;
//   req.ForceCloseReason = THOST_FTDC_FCC_NotForceClose;
//   req.OrderPriceType = priceType;
//   if (mode == Mode::None) {
//     req.TimeCondition = THOST_FTDC_TC_GFD;
//     req.VolumeCondition = THOST_FTDC_VC_AV;
//   } else if (mode == Mode::FAK) {
//     req.TimeCondition = THOST_FTDC_TC_IOC;
//     req.VolumeCondition = THOST_FTDC_VC_AV;
//   } else if (mode == Mode::FOK) {
//     req.TimeCondition = THOST_FTDC_TC_IOC;
//     req.VolumeCondition = THOST_FTDC_VC_CV;
//   }
//   req.ContingentCondition = condition;
//   if (stopPrice != 0) {
//     req.StopPrice = stopPrice;
//   }
//   int ret = _tdApi->ReqOrderInsert(&req, ++_requestId);
//   if (ret != 0) {
//     SPDLOG_ERROR("ret={}", ret);
//   }
//   return ret;
// }

// int Trade::cancelOrder(std::string exchangeId, std::string instrumentId,
//                        std::string orderSysId) {
//   CThostFtdcInputOrderActionField req = {0};
//   strcpy(req.BrokerID, _brokerId.c_str());
//   strcpy(req.InvestorID, _investorId.c_str());
//   strcpy(req.InstrumentID, instrumentId.c_str());
//   strcpy(req.ExchangeID, exchangeId.c_str());
//   strcpy(req.OrderSysID, orderSysId.c_str());
//   req.ActionFlag = THOST_FTDC_AF_Delete;

//   int ret = _tdApi->ReqOrderAction(&req, ++_requestId);
//   if (ret != 0) {
//     SPDLOG_ERROR("ret={}", ret);
//   }
//   return ret;
// }

}  // namespace wss
