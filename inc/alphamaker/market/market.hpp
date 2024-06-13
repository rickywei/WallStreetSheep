#pragma once

namespace am {

class Market {
 public:
  Market() = default;
  virtual ~Market() = default;

  virtual bool init() = 0;
  virtual void release() = 0;
  virtual void connect() = 0;
  virtual void disconnect() = 0;
  virtual void subscribe() = 0;
  virtual void unsubscribe() = 0;
};

}  // namespace am
