#include "IStrategy.hpp"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <range/v3/all.hpp>

#include "../../common/thread.hpp"
#include "../encoding/json.hpp"

namespace wss {

constexpr std::string chFmtTick = "ctpTick:{}";

IStrategy::IStrategy(std::string id) : _id(id) {
  YAML::Node conf = YAML::LoadFile(fmt::format("{}.yaml", _id));
  auto subs = conf["subs"];
  _subTicks = subs["ticks"].as<std::vector<std::string>>();
  _subBars = subs["bars"].as<std::vector<std::string>>();

  _rc = std::make_unique<sw::redis::Redis>(conf["redisAddr"].as<std::string>());
}

void IStrategy::subscribe() {
  if (!_subTicks.empty()) {
    postTask([this]() {
      auto ch = this->_rc->subscriber();
      auto v = this->_subTicks | ranges::views::transform([](std::string s) {
                 return fmt::format(chFmtTick, s);
               }) |
               ranges::to<std::vector>();
      ch.subscribe(v.begin(), v.end());
      ch.on_message([this](std::string channel, std::string msg) {
        SPDLOG_INFO("{}", msg);
        this->onTick(unmarshalTick(msg));
      });
      while (true) {
        ch.consume();
      }
    });
  }
}

}  // namespace wss
