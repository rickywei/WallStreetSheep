#pragma once

#include <unicode/ucnv.h>
#include <unicode/unistr.h>

#include <atomic>
#include <map>
#include <string>

#include "WallStreetSheep/trade/ITrade.hpp"
#include "api/ctp_v6.7.2/ThostFtdcTraderApi.h"

namespace wss {

class ManagerCtp;

class TradeCtp final : public ITrade, public CThostFtdcTraderSpi {
 public:
  friend class ManagerCtp;

  TradeCtp(std::string configPath);
  virtual ~TradeCtp();

  void ReqQrySettlementInfo();
  void ReqQryInvestorPosition();
  void ReqQryInstrument();

  virtual void init() override;
  virtual void start() override;
  virtual void disconnect() override;

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
  std::map<std::string, std::shared_ptr<CThostFtdcInstrumentField>>
      _instruments={};

  [[nodiscard]] int login();
};

}  // namespace wss
