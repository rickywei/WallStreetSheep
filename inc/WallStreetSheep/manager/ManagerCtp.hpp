#pragma once

#include <memory>
#include <string>
#include <vector>

#include "WallStreetSheep/manager/IManager.hpp"
#include "WallStreetSheep/market/MarketCtp.hpp"
#include "WallStreetSheep/trade/TradeCtp.hpp"

namespace wss {

class ManagerCtp final : public IManager {
 public:
  ManagerCtp(std::string configPath);
  virtual ~ManagerCtp();

  virtual void start() override;

 private:
  std::unique_ptr<MarketCtp> _md;
  std::unique_ptr<TradeCtp> _td;

  void subscribe(std::vector<CThostFtdcInstrumentField> instruments);
  void unsubscribe(std::vector<CThostFtdcInstrumentField> instruments);
};

}  // namespace wss
