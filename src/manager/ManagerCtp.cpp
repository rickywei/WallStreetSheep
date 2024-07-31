#include "WallStreetSheep/manager/ManagerCtp.hpp"

#include "WallStreetSheep/market/IMarket.hpp"
#include "WallStreetSheep/market/MarketCtp.hpp"
#include "WallStreetSheep/trade/ITrade.hpp"
#include "WallStreetSheep/trade/TradeCtp.hpp"

void ManagerCtp::start() {
  auto tradeCtp = std::make_shared<wss::TradeCtp>("../config.yaml");
  auto marketCtp = std::make_shared<wss::MarketCtp>("../config.yaml");

  wss::postTask([tradeCtp]() {
    tradeCtp->start();
    SPDLOG_INFO("ctp trade started...");
  });
  std::this_thread::sleep_for(std::chrono::seconds(1));
  dynamic_cast<wss::TradeCtp *>(tradeCtp.get())->ReqQryInstrument();

  wss::postTask([marketCtp]() {
    marketCtp->start();
    SPDLOG_INFO("ctp market started...");
  });

  wss::getGlobalThreadPool()->join();
}