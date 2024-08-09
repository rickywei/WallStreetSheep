#include "IStrategy.hpp"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

namespace wss {

IStrategy::IStrategy(std::shared_ptr<ICtx> ctx, std::string id)
    : _ctx(ctx), _id(id), _logger(newLogger(id)) {
  YAML::Node cfg = YAML::LoadFile(fmt::format("{}.yaml", _id));
  _subs = cfg["subs"].as<std::vector<std::string>>();
}

std::vector<std::string> wss::IStrategy::getSubs() { return _subs; }

}  // namespace wss
