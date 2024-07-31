#pragma once

namespace am {

class Trade {
 public:
  Trade(std::string config_path = "./config.yaml")
      : _config_path(config_path){};
  virtual ~Trade() = default;

  virtual void init() = 0;
  virtual void start() = 0;
  virtual void disconnect() = 0;

 protected:
  const std::string _config_path;
};

}  // namespace am
