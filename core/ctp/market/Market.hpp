#pragma once

#include <sw/redis++/redis++.h>

#include <atomic>
#include <string>
#include <vector>

#include "../ctp_v6.7.2/ThostFtdcMdApi.h"

namespace wss {

class Market final : public CThostFtdcMdSpi {
 public:
  Market();
  virtual ~Market();

  void init();
  void start();

  int subscribe(std::vector<std::string> &instrumentIds);
  int unsubscribe(std::vector<std::string> &instrumentIds);

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
  std::atomic_int _requestId = 0;
  std::string _frontAddr;
  std::string _brokerId;
  std::string _investorId;
  std::string _password;
  std::string _flowPath;
  bool _isUsingUdp;
  bool _isMulticast;
  CThostFtdcMdApi *_mdApi;
  std::unique_ptr<sw::redis::Redis> _rc;
  //   std::unique_ptr<sw::redis::Subscriber> _ch;

  int _login();
};

}  // namespace wss
