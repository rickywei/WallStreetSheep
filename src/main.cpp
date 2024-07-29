#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "alphamaker/market/market.hpp"
#include "alphamaker/market/market_ctp.hpp"
#include "alphamaker/trade/trade.hpp"
#include "alphamaker/trade/trade_ctp.hpp"
#include "spdlog/sinks/daily_file_sink.h"

void replace_default_logger() {
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  auto file_sink = std::make_shared<spdlog::sinks::daily_file_format_sink_mt>(
      "logs/daily.txt", 0, 0);
  auto logger = std::make_shared<spdlog::logger>(
      "default", spdlog::sinks_init_list{console_sink, file_sink});
  spdlog::set_default_logger(logger);
  spdlog::set_level(spdlog::level::debug);
  spdlog::flush_every(std::chrono::seconds(1));
  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%@] [tid=%t] %v");
}

int main() {
  replace_default_logger();
  // std::jthread([]() {
  //   am::Market *market_ctp = new am::MarketCtp("../config.yaml");
  //   market_ctp->Init();
  //   market_ctp->Connect();
  //   SPDLOG_INFO("market finished...");
  // });

  am::Trade *trade_ctp = new am::TradeCtp("../config.yaml");
  std::thread t([trade_ctp]() {
    SPDLOG_INFO("here............1");
    trade_ctp->Init();
    trade_ctp->Connect();
    SPDLOG_INFO("here............2");
  });
  sleep(1);
  SPDLOG_INFO("here............3");

  // dynamic_cast<am::TradeCtp *>(trade_ctp)->ReqQryInvestorPosition();
  dynamic_cast<am::TradeCtp *>(trade_ctp)->ReqQryInstrument();
  if (t.joinable()) t.join();

  return 0;
}