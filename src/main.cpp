#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>
#include <chrono>
#include <iostream>
#include <memory>
#include <thread>

#include "WallStreetSheep/common/common.hpp"

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


  return 0;
}