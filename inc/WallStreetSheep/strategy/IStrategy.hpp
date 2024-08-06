#pragma once

#include <memory>
#include <unordered_set>


class IStrategy {
 public:


  virtual void onInit() = 0;
  virtual void onStop() = 0;
  virtual void onTick() = 0;
  virtual void onBar() = 0;
  virtual void onOrder() = 0;
  virtual void onTrade() = 0;

 protected:
 private:
};