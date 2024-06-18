#pragma once
#include <string>
namespace am {

class Market {
 public:
  Market(std::string config_path = "./config.yaml")
      : _config_path(config_path){};
  virtual ~Market() = default;

  virtual void LoadConfig() = 0;
  virtual void Init() = 0;
  virtual void Connect() = 0;
  virtual void Disconnect() = 0;
  virtual void Subscribe() = 0;
  virtual void Unsubscribe() = 0;

 protected:
  const std::string _config_path;
};

}  // namespace am
