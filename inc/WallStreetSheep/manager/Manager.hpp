#pragma once

#include <memory>
#include <string>
#include <vector>

#include "WallStreetSheep/ctp/Market.hpp"
#include "WallStreetSheep/ctp/Trade.hpp"

namespace wss {

class Manager final {
 public:
  Manager(std::string configPath);

  ~Manager();

  void start();

 private:
  std::unique_ptr<Market> _md;
  std::unique_ptr<Trade> _td;
};

}  // namespace wss
