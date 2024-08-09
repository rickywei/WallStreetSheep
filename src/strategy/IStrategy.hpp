#pragma once
#include <memory>

#include "../common/log.hpp"
#include "../context/ICtx.hpp"

namespace wss {

class ICtx;

class IStrategy : public std::enable_shared_from_this<IStrategy> {
 public:
  IStrategy(std::shared_ptr<ICtx> ctx, std::string id);
  virtual ~IStrategy() = default;

  std::vector<std::string> getSubs();

  virtual void onInit() = 0;
  virtual void onStop() = 0;
  virtual void onTick(std::shared_ptr<CThostFtdcDepthMarketDataField> data) = 0;
  virtual void onBar() = 0;
  virtual void onOrder() = 0;
  virtual void onTrade() = 0;
  virtual void onSchedule() = 0;

 protected:
  std::shared_ptr<ICtx> _ctx;
  std::string _id;
  std::shared_ptr<spdlog::logger> _logger;

 private:
  std::vector<std::string> _subs;
};

}  // namespace wss
