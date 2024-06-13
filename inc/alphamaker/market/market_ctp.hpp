#pragma once

#include <string>

#include "api/ctp_v6.7.2/ThostFtdcMdApi.h"
#include "alphamaker/market/market.hpp"

namespace am {

class MarketCtp final : public Market, public CThostFtdcMdSpi {
  // IMarket
 public:
  virtual bool init() override;
  virtual void release() override;
  virtual void connect() override;
  virtual void disconnect() override;
  virtual void subscribe() override;
  virtual void unsubscribe() override;
  //   virtual void registerSpi() override;

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
  MarketCtp(std::string frontAddr, std::string brokerId, std::string investorId,
            std::string password, std::string flowPath, bool isUsingUdp,
            bool isMulticast);
  virtual ~MarketCtp();

 private:
  [[nodiscoard]] int Login();
  bool IsError(const CThostFtdcRspInfoField *pRspInfo) const;

 private:
  int _requestId = 0;
  const std::string _front_addr;
  const std::string _broker_id;
  const std::string _investor_id;
  const std::string _password;
  const std::string _flowPath;
  const bool _is_using_udp;
  const bool _is_multicast;
  CThostFtdcMdApi *_md_api;
};

}  // namespace am
