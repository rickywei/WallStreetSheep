#pragma once

#include <memory>
#include <pqxx/pqxx>
#include <string>

namespace wss {

class Postgres final {
 public:
  Postgres(std::string path);
  std::unique_ptr<pqxx::work> newWork();

 private:
  std::string _host;
  int _port;
  std::string _user;
  std::string _password;
  std::string _database;
  std::string _dsn;
  std::unique_ptr<pqxx::connection> _conn;
};

}  // namespace wss
