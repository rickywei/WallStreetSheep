#include "WallStreetSheep/manager/Manager.hpp"

#include <spdlog/spdlog.h>

#include <chrono>

#include "WallStreetSheep/common/thread.hpp"

namespace wss {

Manager::Manager(std::string configPath)
    : _md(std::make_unique<Market>(configPath)),
      _td(std::make_unique<Trade>(configPath)) {}

Manager::~Manager() {}

void Manager::start() {
  this->_td->start();
  this->_md->start();

  _td->querySettlementInfo();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  _td->confirmSettlementInfo();
  std::this_thread::sleep_for(std::chrono::seconds(1));
  _td->queryInstrument();

  // SPDLOG_INFO("after");

  getGlobalThreadPool()->join();
}

}  // namespace wss
