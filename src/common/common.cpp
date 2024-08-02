#include "WallStreetSheep/common/common.hpp"

#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/spdlog.h>

#include <boost/asio/post.hpp>

namespace wss {

std::shared_ptr<boost::asio::thread_pool> getGlobalThreadPool() {
  static std::shared_ptr<boost::asio::thread_pool> pool =
      std::make_shared<boost::asio::thread_pool>();
  return pool;
}
void postTask(std::function<void()> &&fn) {
  boost::asio::post(*getGlobalThreadPool(), [f = std::move(fn)]() {
    try {
      f();
    } catch (std::exception &e) {
      SPDLOG_ERROR("task exec failed, exception={}", e.what());
    }
  });
}

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

}  // namespace wss
