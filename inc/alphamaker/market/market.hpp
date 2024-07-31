#pragma once

#include <iostream>

#include "alphamaker/db/postgres.hpp"

namespace am {

class Market {
 public:
  Market(std::string configPath = "./config.yaml")
      : _configPath(configPath) {
        _db=std::make_unique<Postgres>(configPath);
      };
  virtual ~Market() = default;

  virtual void init() = 0;
  virtual void start() = 0;
  virtual void disconnect() = 0;
  virtual void subscribe() = 0;
  virtual void unsubscribe() = 0;

 protected:
  const std::string _configPath;
  std::unique_ptr<Postgres> _db;
};

}  // namespace am
