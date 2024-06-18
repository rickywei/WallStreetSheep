#include <spdlog/spdlog.h>

#include <iostream>
#include <memory>
#include <thread>

#include "alphamaker/market/market.hpp"
#include "alphamaker/market/market_ctp.hpp"
#include "alphamaker/trade/trade.hpp"
#include "alphamaker/trade/trade_ctp.hpp"

int main() {
  // std::jthread([]() {
  //   am::Market *market_ctp = new am::MarketCtp("../config.yaml");
  //   market_ctp->Init();
  //   market_ctp->Connect();
  //   spdlog::info("market finished...");
  // });

  am::Trade *trade_ctp = new am::TradeCtp("../config.yaml");
  std::jthread([trade_ctp]() {
    spdlog::info("here............1");
    trade_ctp->Init();
    trade_ctp->Connect();
    spdlog::info("here............2");
  });
  sleep(1);
  spdlog::info("here............3");

  dynamic_cast<am::TradeCtp *>(trade_ctp)->ReqQryInvestorPosition();

  return 0;
}