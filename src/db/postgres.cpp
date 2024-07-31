#include "alphamaker/db/postgres.hpp"

#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <boost/asio/post.hpp>
#include <memory>

namespace am {

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
}

std::unique_ptr<pqxx::work> Postgres::newWork() {
  auto conn=new pqxx::connection(_dsn);
  return std::make_unique<pqxx::work>(*conn);
}

}  // namespace am
