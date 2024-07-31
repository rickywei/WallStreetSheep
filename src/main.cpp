#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "WallStreetSheep/common/common.hpp"
#include "WallStreetSheep/market/IMarket.hpp"
#include "WallStreetSheep/market/MarketCtp.hpp"
#include "WallStreetSheep/trade/ITrade.hpp"
#include "WallStreetSheep/trade/TradeCtp.hpp"
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
  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%@] [%!] [tid=%t] %v");
}

int main() {
  replace_default_logger();

  // wss::postTask([]() {
  //   auto market_ctp = std::make_unique<wss::MarketCtp>("../config.yaml");
  //   market_ctp->start();
  //   SPDLOG_INFO("ctp market started...");
  // });


    auto trade_ctp = std::make_shared<wss::TradeCtp>("../config.yaml");
  wss::postTask([trade_ctp]() {
    trade_ctp->start();
    SPDLOG_INFO("ctp trade started...");
  });
  std::this_thread::sleep_for(std::chrono::seconds(1));
    dynamic_cast<wss::TradeCtp *>(trade_ctp.get())->ReqQryInstrument();

  // dynamic_cast<wss::TradeCtp *>(trade_ctp)->ReqQryInvestorPosition();

  wss::getGlobalThreadPool()->join();
  return 0;
}