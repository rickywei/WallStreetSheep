#include "db/Postgres.hpp"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <atomic>

#include "common/thread.hpp"

namespace wss {

Postgres::Postgres(std::string path) {
  YAML::Node config = YAML::LoadFile(path);
  YAML::Node postgres_config = config["postgres"];
  _host = postgres_config["host"].as<std::string>();
  _port = postgres_config["port"].as<int>();
  _user = postgres_config["user"].as<std::string>();
  _password = postgres_config["password"].as<std::string>();
  _database = postgres_config["database"].as<std::string>();
  _dsn = fmt::format("host={} port={} user={} password={} dbname={}", _host,
                     _port, _user, _password, _database);

  _startWriteQueue();
  _startReadQueue();
}

void Postgres::_startWriteQueue() {
  for (int i = 0; i < _n; ++i) {
    postTask([this]() {
      auto _conn = pqxx::connection(this->_dsn);
      while (true) {
        try {
          std::function<void(pqxx::connection&)> f;
          this->_writeQueue.wait_dequeue(f);
          if (!_conn.is_open()) {
            _conn = pqxx::connection(_dsn);
          }
          f(_conn);
        } catch (std::exception& e) {
          SPDLOG_ERROR("write pg failed, exception={}", e.what());
        }
      }
    });
  }
}

void Postgres::_startReadQueue() {}
}  // namespace wss
