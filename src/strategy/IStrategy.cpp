#include "IStrategy.hpp"

// #include <spdlog/spdlog.h>
// #include <yaml-cpp/yaml.h>

#include <vector>

namespace wss {

IStrategy::IStrategy(std::string name) : _name(name) {
  // YAML::Node config = YAML::LoadFile(fmt::format("./{}.yaml", _name));

  // auto subs = config["subs"].as<std::string>();
  // mgr->strategySubscribe(shared_from_this(), subs);

  //   onInit();
}

IStrategy::~IStrategy() {}

void IStrategy::longOpen() {}

void IStrategy::longClose() {}

void IStrategy::shortOpen() {}

void IStrategy::shortClose() {}

void IStrategy::backtest() {}

void IStrategy::simulate() {}

void IStrategy::online() {}

}  // namespace wss
