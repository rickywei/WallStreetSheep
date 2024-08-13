#include "Example.hpp"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

namespace wss {

Example::Example(std::string id) : IStrategy(id) {}

void Example::onInit() { SPDLOG_INFO(""); }
void Example::onStop() { SPDLOG_INFO(""); }
void Example::onTick(std::unique_ptr<Tick> tick) { SPDLOG_INFO(""); }
void Example::onBar(std::unique_ptr<Bar> bar) { SPDLOG_INFO(""); }
void Example::onOrder() { SPDLOG_INFO(""); }
void Example::onTrade() { SPDLOG_INFO(""); }
void Example::onSchedule() { SPDLOG_INFO(""); }

}  // namespace wss