#include "WallStreetSheep/manager/Manager.hpp"

#include <spdlog/spdlog.h>

#include <chrono>
#include <range/v3/all.hpp>

#include "WallStreetSheep/common/thread.hpp"

namespace wss {

Manager::Manager(std::string configPath)
    : _md(std::make_unique<Market>(configPath)),
      _td(std::make_unique<Trade>(configPath)) {}

Manager::~Manager() {}

void Manager::start() {
  this->_td->start();
  this->_md->start();

  auto instruments =
      _td->instruments | ranges::views::keys | ranges::to<std::unordered_set>;
  _md->subscribe(instruments);

  getGlobalThreadPool()->join();
}

}  // namespace wss
