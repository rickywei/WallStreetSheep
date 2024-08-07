#include "manager/Manager.hpp"

#include <dlfcn.h>
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <chrono>
#include <range/v3/all.hpp>

#include "common/thread.hpp"

namespace wss {

Manager::Manager(std::string configPath)
    : _md(std::make_unique<Market>(configPath)),
      _td(std::make_unique<Trade>(configPath)) {}

Manager::~Manager() {}

void Manager::start() {
  // this->_td->start();
  // this->_md->start();

  // auto instruments =
  //     _td->instruments | ranges::views::keys |
  //     ranges::to<std::unordered_set>;
  // _md->subscribe(instruments);

  loadStrategy("");

  getGlobalThreadPool()->join();
}

void Manager::loadStrategy(std::string name) {
  void* handle = dlopen("./libWallStreetSheepStrategy.so", RTLD_NOW);
  if (handle == nullptr) {
    SPDLOG_ERROR("{}", dlerror());
    throw "handle is nullptr";
  }
  using ft = std::unique_ptr<IStrategy>(std::string);
  ft* create = reinterpret_cast<ft*>(dlsym(handle, "create"));
  if (create == nullptr) {
    SPDLOG_ERROR("create is nullptr");
    throw "create is nullptr";
  }

  YAML::Node config = YAML::LoadFile("./config.yaml");
  YAML::Node strategyConfig = config["strategy"];
  if (name == "") {
    for (auto n : strategyConfig["names"]) {
      auto name = n.as<std::string>();
      if (_name2strategy.contains(name)) continue;
      try {
        _name2strategy[name] = create(name);
      } catch (std::exception& e) {
        SPDLOG_ERROR("exception={}", e.what());
      }
    }
  } else {
    try {
      /* code */
    } catch (const std::exception& e) {
      SPDLOG_ERROR("exception={}", e.what());
    }
  }

  dlclose(handle);
}

void Manager::strategySubscribe(std::shared_ptr<IStrategy> s,
                                std::vector<std::string>& subs) {}
}  // namespace wss
