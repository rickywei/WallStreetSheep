
#include "../common/log.hpp"

#include <spdlog/sinks/daily_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace wss {

void replace_default_logger(std::string name) {
  auto logger = newLogger(name);
  spdlog::flush_every(std::chrono::seconds(1));
  spdlog::set_default_logger(logger);
}

std::shared_ptr<spdlog::logger> newLogger(std::string name) {
  auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
  auto file_sink = std::make_shared<spdlog::sinks::daily_file_format_sink_mt>(
      fmt::format("logs/daily{}.log", name == "" ? "" : "-" + name), 0, 0);
  auto logger = std::make_shared<spdlog::logger>(
      name, spdlog::sinks_init_list{console_sink, file_sink});
  // TODO check compile flag
  logger->set_level(spdlog::level::debug);
  logger->set_pattern("[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] [%@] [%!] [tid=%t] %v");

  return logger;
}

}  // namespace wss