#include "IStrategy.hpp"

namespace wss {

class Example : public IStrategy {
 public:
  Example(std::shared_ptr<ICtx> ctx, std::string name);

  virtual void onInit() override;
  virtual void onStop() override;
  virtual void onTick(std::shared_ptr<CThostFtdcDepthMarketDataField> data) override;
  virtual void onBar() override;
  virtual void onOrder() override;
  virtual void onTrade() override;
  virtual void onSchedule() override;
};

}  // namespace wss