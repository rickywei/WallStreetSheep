#pragma once

#include <sw/redis++/redis++.h>
#include <unicode/ucnv.h>
#include <unicode/unistr.h>

#include <atomic>
#include <map>
#include <string>

#include "ctp_v6.7.2/ThostFtdcTraderApi.h"

namespace wss {

class Trade final : public CThostFtdcTraderSpi {
 public:
  Trade(std::string str);
  virtual ~Trade();

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
  virtual void OnErrRtnOrderInsert(CThostFtdcInputOrderField *pInputOrder,
                                   CThostFtdcRspInfoField *pRspInfo) override;
  virtual void OnRtnOrder(CThostFtdcOrderField *pOrder) override;
  virtual void OnRtnTrade(CThostFtdcTradeField *pTrade) override;
  virtual void OnRspOrderAction(
      CThostFtdcInputOrderActionField *pInputOrderAction,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  virtual void OnErrRtnOrderAction(CThostFtdcOrderActionField *pOrderAction,
                                   CThostFtdcRspInfoField *pRspInfo) override;

 private:
  std::atomic_bool _isLogin = false;
  std::string _frontAddr;
  std::string _brokerId;
  std::string _investorId;
  std::string _password;
  std::string _redisAddr;
  std::string _flowPath;
  std::unique_ptr<sw::redis::Redis> _rc;
  CThostFtdcTraderApi *_tdApi;
  int _frontId;
  int _sessionId;
  int _maxOrderRef;

  void _daemon();
  int _getRequestId();
  void _login();
  void _querySettlementInfo();
  void _confirmSettlementInfo();
  void _queryInstrument();
  void _queryInvestorPosition();
  void _order(std::string str);
  void _cancel(std::string str);
};

}  // namespace wss
