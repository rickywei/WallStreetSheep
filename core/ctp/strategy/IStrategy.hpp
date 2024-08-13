#pragma once
#include <sw/redis++/redis++.h>

#include <memory>

#include "../../common/data.hpp"
#include "../../common/log.hpp"

namespace wss {

class IStrategy {
 public:
  IStrategy(std::string id);
  virtual ~IStrategy() = default;

  void subscribe();

  virtual void onInit() = 0;
  virtual void onStop() = 0;
  virtual void onTick(std::unique_ptr<Tick> tick) = 0;
  virtual void onBar(std::unique_ptr<Bar> bar) = 0;
  virtual void onOrder() = 0;
  virtual void onTrade() = 0;
  virtual void onSchedule() = 0;

 protected:
  std::string _id;
  std::vector<std::string> _subTicks;
  std::vector<std::string> _subBars;
  std::unique_ptr<sw::redis::Redis> _rc;

 private:
};

}  // namespace wss
