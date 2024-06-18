#pragma once

namespace am {

class Trade {
 public:
  Trade(std::string config_path = "./config.yaml")
      : _config_path(config_path){};
  virtual ~Trade() = default;

  virtual void LoadConfig() = 0;
  virtual void Init() = 0;
  virtual void Connect() = 0;
  virtual void Disconnect() = 0;

 protected:
  const std::string _config_path;
};

}  // namespace am
