#pragma once

#include <spdlog/spdlog.h>

#include <memory>

namespace wss {

void replace_default_logger();

std::shared_ptr<spdlog::logger> newLogger(std::string name);

}  // namespace wss