#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../ctp/Market.hpp"
#include "../ctp/Trade.hpp"
#include "../strategy/IStrategy.hpp"

namespace wss {

class IStrategy;

class Manager final : public std::enable_shared_from_this<Manager> {
 public:
  Manager(std::string configPath);

  ~Manager();

  void start();
  void loadStrategy(std::string name);
  void strategySubscribe(std::shared_ptr<IStrategy> s,
                         std::vector<std::string>& subs);

 private:
  std::unique_ptr<Market> _md;
  std::unique_ptr<Trade> _td;
  std::unordered_map<std::string, std::unique_ptr<IStrategy>> _name2strategy;
  std::unordered_map<std::string,
                     std::unordered_set<std::shared_ptr<IStrategy>>>
      _instrument2name;
};

}  // namespace wss
