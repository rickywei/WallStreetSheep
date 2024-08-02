#include "WallStreetSheep/manager/ManagerCtp.hpp"

#include <spdlog/spdlog.h>

#include <chrono>
#include <thread>

#include "WallStreetSheep/common/common.hpp"

namespace wss {

ManagerCtp::ManagerCtp(std::string configPath)
    : _md(std::make_unique<MarketCtp>(configPath)),
      _td(std::make_unique<TradeCtp>(configPath)) {}

ManagerCtp::~ManagerCtp() {}

void ManagerCtp::start() {
  postTask([this]() { this->_td->start(); });
  _td->_inited.wait(false);
  // SPDLOG_INFO("after");
  // std::this_thread::sleep_for(std::chrono::seconds(1));
  _td->ReqQryInstrument();


  // postTask([this]() { this->_md->start(); });
  // _md->_inited.wait(false);

  // _md->subscribe();
  // // _md->subscribe(ins);

  getGlobalThreadPool()->join();
}

}  // namespace wss
