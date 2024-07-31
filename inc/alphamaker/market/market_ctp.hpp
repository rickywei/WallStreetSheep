#pragma once

#include <string>

#include "alphamaker/market/market.hpp"
#include "api/ctp_v6.7.2/ThostFtdcMdApi.h"

namespace am {

class MarketCtp final : public Market, public CThostFtdcMdSpi {
 public:
  virtual void init() override;
  virtual void start() override;
  virtual void disconnect() override;
  virtual void subscribe() override;
  virtual void unsubscribe() override;

  virtual void OnFrontConnected() override;
  virtual void OnFrontDisconnected(int nReason) override;
  virtual void OnHeartBeatWarning(int nTimeLapse) override;
  virtual void OnRspUserLogin(CThostFtdcRspUserLoginField *pRspUserLogin,
                              CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                              bool bIsLast) override;
  virtual void OnRspUserLogout(CThostFtdcUserLogoutField *pUserLogout,
                               CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                               bool bIsLast) override;
  virtual void OnRspQryMulticastInstrument(
      CThostFtdcMulticastInstrumentField *pMulticastInstrument,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  virtual void OnRspError(CThostFtdcRspInfoField *pRspInfo, int nRequestID,
                          bool bIsLast) override;
  virtual void OnRspSubMarketData(
      CThostFtdcSpecificInstrumentField *pSpecificInstrument,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  virtual void OnRspUnSubMarketData(
      CThostFtdcSpecificInstrumentField *pSpecificInstrument,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  virtual void OnRspSubForQuoteRsp(
      CThostFtdcSpecificInstrumentField *pSpecificInstrument,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  virtual void OnRspUnSubForQuoteRsp(
      CThostFtdcSpecificInstrumentField *pSpecificInstrument,
      CThostFtdcRspInfoField *pRspInfo, int nRequestID, bool bIsLast) override;
  virtual void OnRtnDepthMarketData(
      CThostFtdcDepthMarketDataField *pDepthMarketData) override;
  virtual void OnRtnForQuoteRsp(
      CThostFtdcForQuoteRspField *pForQuoteRsp) override;

 public:
  MarketCtp(std::string config_path);
  virtual ~MarketCtp();

 private:
  [[nodiscard]] int login();

 private:
  int _requestId = 0;
  std::string _frontAddr;
  std::string _brokerId;
  std::string _investorId;
  std::string _password;
  std::string _flowPath;
  bool _isUsingUdp;
  bool _isMulticast;
  CThostFtdcMdApi *_mdApi;
};

}  // namespace am
