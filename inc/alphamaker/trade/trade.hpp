#pragma once

namespace am {


class Trade {
 public:
  Trade() = default;
  virtual ~Trade();

  virtual bool init() = 0;
  virtual void release() = 0;
  virtual void connect() = 0;
  virtual void disconnect() = 0;
    
};

}  // namespace am
