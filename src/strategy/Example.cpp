#include "Example.hpp"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

namespace wss {

Example::Example(std::shared_ptr<ICtx> ctx, std::string id)
    : IStrategy(ctx, id) {}

void Example::onInit() { SPDLOG_LOGGER_INFO(_logger, ""); }
void Example::onStop() { SPDLOG_LOGGER_INFO(_logger, ""); }
void Example::onTick(std::shared_ptr<CThostFtdcDepthMarketDataField> data) {
  SPDLOG_LOGGER_INFO(_logger, "");
}
void Example::onBar() { SPDLOG_LOGGER_INFO(_logger, ""); }
void Example::onOrder() { SPDLOG_LOGGER_INFO(_logger, ""); }
void Example::onTrade() { SPDLOG_LOGGER_INFO(_logger, ""); }
void Example::onSchedule() { SPDLOG_LOGGER_INFO(_logger, ""); }

}  // namespace wss