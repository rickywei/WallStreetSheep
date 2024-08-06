#pragma once

#include <atomic>
#include <string>
#include <unordered_set>

#include "WallStreetSheep/db/Postgres.hpp"

namespace wss {

class ICtp {
 public:
  ICtp(std::string configPath)
      : _configPath(configPath), _db(std::make_unique<Postgres>(configPath)) {};
  virtual ~ICtp() = default;

  virtual void init() = 0;
  virtual void start() = 0;

 protected:
  const std::string _configPath;
  std::unique_ptr<Postgres> _db;
  std::atomic_bool _logged = false;
};

}  // namespace wss
