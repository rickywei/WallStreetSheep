#pragma once

#include <iostream>

#include "WallStreetSheep/db/Postgres.hpp"

namespace wss {

class IMarket {
 public:
  IMarket(std::string configPath = "./config.yaml")
      : _configPath(configPath) {
        _db=std::make_unique<Postgres>(configPath);
      };
  virtual ~IMarket() = default;

  virtual void init() = 0;
  virtual void start() = 0;
  virtual void disconnect() = 0;
  virtual void subscribe() = 0;
  virtual void unsubscribe() = 0;

 protected:
  const std::string _configPath;
  std::unique_ptr<Postgres> _db;
};

}  // namespace wss
