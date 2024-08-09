#pragma once

#include <memory>
#include <string>
#include <vector>

#include "../context/CtxBacktest.hpp"
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
  void subscribe(std::vector<std::string>);

 private:
  Market _md;
  Trade _td;
  CtxBacktest _ctxBacktest;
  std::unordered_map<std::string,
                     std::unordered_set<std::shared_ptr<IStrategy>>>
      _instrument2name;
  std::shared_ptr<ICtx> _backtest = std::make_shared<CtxBacktest>();
  std::shared_ptr<ICtx> _simulate = std::make_shared<CtxBacktest>();
  std::shared_ptr<ICtx> _live = std::make_shared<CtxBacktest>();
  std::unordered_map<
      std::string, std::unordered_map<std::string, std::unique_ptr<IStrategy>>>
      _ums ;
};

}  // namespace wss
