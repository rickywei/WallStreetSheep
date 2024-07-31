#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "alphamaker/common/common.hpp"
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
  spdlog::set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%@] [%!] [tid=%t] %v");
}

int main() {
  replace_default_logger();

  am::postTask([]() {
    auto market_ctp = std::make_unique<am::MarketCtp>("../config.yaml");
    market_ctp->start();
    SPDLOG_INFO("ctp market started...");
  });
  // am::postTask([]() {
  //   auto trade_ctp = std::make_unique<am::TradeCtp>("../config.yaml");
  //   trade_ctp->start();
  //   SPDLOG_INFO("ctp trade started...");
  //   // dynamic_cast<am::TradeCtp *>(trade_ctp.get())->ReqQryInstrument();
  // });

  // dynamic_cast<am::TradeCtp *>(trade_ctp)->ReqQryInvestorPosition();

  am::getGlobalThreadPool()->join();
  return 0;
}