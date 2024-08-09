#include "manager/Manager.hpp"

#include <dlfcn.h>
#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <chrono>
#include <range/v3/all.hpp>

#include "../common/thread.hpp"
#include "Manager.hpp"

namespace wss {

Manager::Manager(std::string configPath) : _md(configPath), _td(configPath) {
  _ums["backtest"] =
      std::unordered_map<std::string, std::unique_ptr<IStrategy>>();
  _ums["simulate"] =
      std::unordered_map<std::string, std::unique_ptr<IStrategy>>();
  _ums["live"] = std::unordered_map<std::string, std::unique_ptr<IStrategy>>();
}

Manager::~Manager() {}

void Manager::start() {
  this->_td.start();
  this->_md.start();

  loadStrategy("");

  getGlobalThreadPool()->join();
}

void Manager::loadStrategy(std::string id) {
  void* handle = dlopen("./libWallStreetSheepStrategy.so", RTLD_NOW);
  if (handle == nullptr) {
    SPDLOG_ERROR("{}", dlerror());
    throw "handle is nullptr";
  }
  using ft = std::unique_ptr<IStrategy>(std::shared_ptr<ICtx>, std::string);
  ft* create = reinterpret_cast<ft*>(dlsym(handle, "create"));
  if (create == nullptr) {
    SPDLOG_ERROR("create is nullptr");
    throw "create is nullptr";
  }

  YAML::Node config = YAML::LoadFile("./config.yaml");
  YAML::Node strategyConfig = config["strategy"];
  for (auto& [t, um] : _ums) {
    for (auto n : strategyConfig[t]) {
      auto curId = n.as<std::string>();
      if ((id == "" && um.contains(id)) || (id != "" && id != curId)) continue;
      try {
        auto sp = create(_backtest, curId);
        // TODO may subscribe all market data in future,
        // needed currently
        sp->onInit();
        // subscribe(sp->getSubs());
        um.insert({curId, std::move(sp)});
      } catch (std::exception& e) {
        SPDLOG_ERROR("exception={}", e.what());
      }
    }
  }

  dlclose(handle);
}

void Manager::subscribe(std::vector<std::string> instrumentIds) {
  _md.subscribe(instrumentIds);
}

}  // namespace wss
