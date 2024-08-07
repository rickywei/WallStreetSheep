#pragma once

#include <unicode/ucnv.h>
#include <unicode/unistr.h>

#include <atomic>
#include <map>
#include <string>

#include "../ctp/ICtp.hpp"
#include "../ctp/utils.hpp"
#include "../ctp_v6.7.2/ThostFtdcTraderApi.h"

namespace wss {

class Trade final : public ICtp, public CThostFtdcTraderSpi {
 public:
  std::map<std::string, std::shared_ptr<InstrumentData>>
      instruments = {};

  Trade(std::string configPath);
  virtual ~Trade();

  virtual void init() override;
  virtual void start() override;

  virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                          bool bIsLast) override;
  virtual void OnFrontConnected() override;
  virtual void OnFrontDisconnected(int nReason) override;
  virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                              CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                              bool bIsLast) override;
  virtual void OnRspQrySettlementInfo(
      CThostFtdcSettlementInfoField *pSettlementInfo,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  virtual void OnRspSettlementInfoConfirm(
      CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument,
                                  CThostFtdcRspInfoField *pRspInfo,
                                  int nRequestID, bool bIsLast) override;
  virtual void OnRspQryInvestorPosition(
      CThostFtdcInvestorPositionField *pInvestorPosition,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder,
                                CThostFtdcRspInfoField *pRspInfo,
                                int nRequestID, bool bIsLast) override;
  virtual void OnRspOrderAction(
      CThostFtdcInputOrderActionField *pInputOrderAction,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  virtual void OnRtnOrder(CThostFtdcOrderField *pOrder) override;
  virtual void OnRtnTrade(CThostFtdcTradeField *pTrade) override;
  virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder,
                                   CThostFtdcRspInfoField *pRspInfo) override;
  virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction,
                                   CThostFtdcRspInfoField *pRspInfo) override;

  int querySettlementInfo();
  int queryInvestorPosition();
  int queryInstrument();
  int confirmSettlementInfo();
  int order(
      std::string exchangeId, std::string instrumentId, int volume,
      double limitPrice, double stopPrice, TThostFtdcDirectionType direction,
      TThostFtdcOffsetFlagType offset, TThostFtdcOrderPriceTypeType priceType,
      TThostFtdcContingentConditionType condition = THOST_FTDC_CC_Immediately,
      Mode mode = Mode::None);
  int cancelOrder(std::string exchangeId, std::string instrumentId,
                  std::string orderSysId);

 private:
  std::atomic_int _requestId = 0;
  std::string _frontAddr;
  std::string _brokerId;
  std::string _investorId;
  std::string _password;
  std::string _flowPath;
  CThostFtdcTraderApi *_tdApi;
  int _frontId;
  int _sessionId;
  int _maxOrderRef;
  int _tradingDay;

  [[nodiscard]] int login();
};

}  // namespace wss
