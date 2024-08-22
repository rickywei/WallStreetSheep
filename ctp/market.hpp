#pragma once

#include <sw/redis++/redis++.h>

#include <atomic>
#include <string>

#include "ctp_v6.7.2/ThostFtdcMdApi.h"

namespace wss {

class Market final : public CThostFtdcMdSpi {
 public:
  Market(std::string str);
  virtual ~Market();

  void init();
  void start();

  virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                          bool bIsLast) override;
  virtual void OnFrontConnected() override;
  virtual void OnFrontDisconnected(int nReason) override;
  virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                              CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                              bool bIsLast) override;
  virtual void OnRspSubMarketData(
      CThostFtdcSpecificInstrumentField *pSpecificInstrument,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  virtual void OnRtnDepthMarketData(
      CThostFtdcDepthMarketDataField *pDepthMarketData) override;
  virtual void OnRspUnSubMarketData(
      CThostFtdcSpecificInstrumentField *pSpecificInstrument,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;

 private:
  std::atomic_bool _isLogin = false;
  std::string _frontAddr;
  std::string _brokerId;
  std::string _investorId;
  std::string _password;
  std::string _flowPath;
  bool _isUsingUdp;
  bool _isMulticast;
  std::string _redisAddr;
  std::unique_ptr<sw::redis::Redis> _rc;
  CThostFtdcMdApi *_mdApi;

  int _getRequestId();
  void _daemon();
  void _login();
  void _subscribe(std::string str);
  void _unsubscribe(std::string str);
};

}  // namespace wss