#pragma once

#include <concurrentqueue/moodycamel/blockingconcurrentqueue.h>

#include <memory>
#include <unordered_map>
#include <unordered_set>

#include "../ctp_v6.7.2/ThostFtdcUserApiStruct.h"
#include "../strategy/IStrategy.hpp"

namespace wss {

class IStrategy;

class ICtx {
 public:
  ICtx() = default;
  virtual ~ICtx() = default;

  CThostFtdcInputOrderField* getOrder();
  CThostFtdcInputOrderActionField* getCancel();
  void subTick(std::shared_ptr<IStrategy> sp, std::vector<std::string>& ids);
  void subBar(std::shared_ptr<IStrategy> sp, std::vector<std::string>& ids);
  void onTick(std::string id,
              std::shared_ptr<CThostFtdcDepthMarketDataField> data);
  void onBar(std::string id,
             std::shared_ptr<CThostFtdcDepthMarketDataField> data);

  virtual void longOpen() = 0;
  virtual void longClose() = 0;
  virtual void shortOpen() = 0;
  virtual void shortClose() = 0;

 private:
  moodycamel::BlockingConcurrentQueue<CThostFtdcInputOrderField*> _orderQueue;
  moodycamel::BlockingConcurrentQueue<CThostFtdcInputOrderActionField*>
      _cancelQueue;
  std::unordered_map<std::string,
                     std::unordered_set<std::shared_ptr<IStrategy>>>
      _tickMap;
  std::unordered_map<std::string,
                     std::unordered_set<std::shared_ptr<IStrategy>>>
      _barMap;
};

}  // namespace wss
