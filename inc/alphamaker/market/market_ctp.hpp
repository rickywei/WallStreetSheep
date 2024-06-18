#pragma once

#include <string>

#include "alphamaker/market/market.hpp"
#include "api/ctp_v6.7.2/ThostFtdcMdApi.h"

namespace am {

class MarketCtp final : public Market, public CThostFtdcMdSpi {
  // Market
 public:
  virtual void LoadConfig() override;
  virtual void Init() override;
  virtual void Connect() override;
  virtual void Disconnect() override;
  virtual void Subscribe() override;
  virtual void Unsubscribe() override;

  // CThostFtdcMdSpi
 public:
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
  [[nodiscoard]] int Login();
  bool IsError(const CThostFtdcRspInfoField *pRspInfo) const;

 private:
  int _requestId = 0;
  std::string _front_addr;
  std::string _broker_id;
  std::string _investor_id;
  std::string _password;
  std::string _flowPath;
  bool _is_using_udp;
  bool _is_multicast;
  CThostFtdcMdApi *_md_api;
  bool is_connect;

};

}  // namespace am
