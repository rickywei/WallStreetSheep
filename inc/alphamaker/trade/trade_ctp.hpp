#pragma once

#include <unicode/ucnv.h>
#include <unicode/unistr.h>

#include <string>

#include "alphamaker/trade/trade.hpp"
#include "api/ctp_v6.7.2/ThostFtdcTraderApi.h"

namespace am {

class TradeCtp final : public Trade, public CThostFtdcTraderSpi {
  // Trade
 public:
  virtual void LoadConfig() override;
  virtual void Init() override;
  virtual void Connect() override;
  virtual void Disconnect() override;

  // CThostFtdcTraderSpi
 public:
  virtual void OnFrontConnected() override;
  virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                              CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                              bool bIsLast) override;
  virtual void OnRspSettlementInfoConfirm(
      CThostFtdcSettlementInfoConfirmField *pSettlementInfoConfirm,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  virtual void OnRspQryInstrument(CThostFtdcInstrumentField *pInstrument,
                                  CThostFtdcRspInfoField *pRspInfo,
                                  int nRequestID, bool bIsLast) override;
  virtual void OnRspQryTradingAccount(
      CThostFtdcTradingAccountField *pTradingAccount,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  virtual void OnRspQryInvestorPosition(
      CThostFtdcInvestorPositionField *pInvestorPosition,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  virtual void OnRspOrderInsert(CThostFtdcInputOrderField *pInputOrder,
                                CThostFtdcRspInfoField *pRspInfo,
                                int nRequestID, bool bIsLast) override;
  virtual void OnRspOrderAction(
      CThostFtdcInputOrderActionField *pInputOrderAction,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                          bool bIsLast) override;
  virtual void OnFrontDisconnected(int nReason) override;
  virtual void OnHeartBeatWarning(int nTimeLapse) override;
  virtual void OnRtnOrder(CThostFtdcOrderField *pOrder) override;
  virtual void OnRtnTrade(CThostFtdcTradeField *pTrade) override;

 public:
  TradeCtp(std::string config_path);
  virtual ~TradeCtp();
  void ReqQrySettlementInfo();
  void ReqQryInvestorPosition();
  void ReqQryInstrument();

 private:
  [[nodiscoard]] int Login();

 private:
  int _requestId = 0;
  std::string _front_addr;
  std::string _broker_id;
  std::string _investor_id;
  std::string _password;
  std::string _flowPath;
  CThostFtdcTraderApi *_td_api;
  int _front_id;
  int _session_id;
  int _max_order_ref;
  int _trading_day;
};

}  // namespace am


