#pragma once

#include <memory>
#include <unordered_set>


namespace wss {


class IStrategy : std::enable_shared_from_this<IStrategy> {
 public:
  IStrategy(std::string name);
  virtual ~IStrategy();

  void longOpen();
  void longClose();
  void shortOpen();
  void shortClose();
  void backtest();
  void simulate();
  void online();

  // virtual void onInit() = 0;
  // virtual void onStop() = 0;
  // virtual void onTick() = 0;
  // virtual void onBar() = 0;
  // virtual void onOrder() = 0;
  // virtual void onTrade() = 0;
  // virtual void onSchedule() = 0;

 protected:
 private:
  std::string _name;
  std::unordered_set<std::string> _filters;
};

}  // namespace wss
