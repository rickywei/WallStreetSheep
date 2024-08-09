#pragma once

#include "../context/ICtx.hpp"

namespace wss {

class CtxBacktest : public ICtx {
 public:
  CtxBacktest() = default;
  virtual ~CtxBacktest() = default;

  virtual void longOpen() override;
  virtual void longClose() override;
  virtual void shortOpen() override;
  virtual void shortClose() override;
};

}  // namespace wss
