#pragma once

#include <sw/redis++/redis++.h>
#include <unicode/ucnv.h>
#include <unicode/unistr.h>

#include <atomic>
#include <map>
#include <string>

#include "../ctp_v6.7.2/ThostFtdcTraderApi.h"

namespace wss {

class Trade final : public CThostFtdcTraderSpi {
 public:
  Trade();
  virtual ~Trade();

  void init();
  void start();

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
  std::unique_ptr<sw::redis::Redis> _rc;

  int _login();
  int _querySettlementInfo();
  int _queryInvestorPosition();
  int _queryInstrument();
  int _confirmSettlementInfo();
  //   int order(
  //       std::string exchangeId, std::string instrumentId, int volume,
  //       double limitPrice, double stopPrice, TThostFtdcDirectionType
  //       direction, TThostFtdcOffsetFlagType offset,
  //       TThostFtdcOrderPriceTypeType priceType,
  //       TThostFtdcContingentConditionType condition =
  //       THOST_FTDC_CC_Immediately, Mode mode = Mode::None);
  //   int cancelOrder(std::string exchangeId, std::string instrumentId,
  //                   std::string orderSysId);
  
};

}  // namespace wss
