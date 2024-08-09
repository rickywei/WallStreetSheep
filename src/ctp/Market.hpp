#pragma once

#include "../ctp/ICtp.hpp"
#include "../ctp_v6.7.2/ThostFtdcMdApi.h"

namespace wss {

class Market final : public ICtp, public CThostFtdcMdSpi {
 public:
  Market(std::string configPath);
  virtual ~Market();

  virtual void init() override;
  virtual void start() override;

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

  [[nodiscard]] int login();
};

}  // namespace wss
