#pragma once

#include "WallStreetSheep/db/Postgres.hpp"

namespace wss {

class ITrade {
 public:
  ITrade(std::string configPath)
      : _configPath(configPath), _db(std::make_unique<Postgres>(configPath)) {};
  virtual ~ITrade() = default;

  virtual void init() = 0;
  virtual void start() = 0;
  virtual void disconnect() = 0;

 protected:
  const std::string _configPath;
  std::unique_ptr<Postgres> _db;
  std::atomic_bool _inited = false;
  std::atomic_bool _logged = false;
};

}  // namespace wss
