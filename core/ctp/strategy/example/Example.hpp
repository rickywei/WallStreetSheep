#include "../IStrategy.hpp"

namespace wss {

class Example : public IStrategy {
 public:
  Example(std::string id);

  virtual void onInit() override;
  virtual void onStop() override;
  virtual void onTick(std::unique_ptr<Tick> tick) override;
  virtual void onBar(std::unique_ptr<Bar> bar) override;
  virtual void onOrder() override;
  virtual void onTrade() override;
  virtual void onSchedule() override;
};

}  // namespace wss