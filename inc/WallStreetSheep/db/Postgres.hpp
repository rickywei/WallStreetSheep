#pragma once

#include <concurrentqueue/moodycamel/blockingconcurrentqueue.h>
#include <spdlog/spdlog.h>

#include <functional>
#include <pqxx/pqxx>
#include <string>

namespace wss {

class Postgres final {
 public:
  Postgres(std::string path);

  template <typename... Args>
  void write(std::string q, Args... args) const {
    _writeQueue.enqueue([q = std::move(q), ... args = std::forward<Args>(args)](
                            pqxx::connection& conn) {
      auto work = pqxx::work(conn);
      auto result = work.exec_params(q, args...);
      work.commit();
    });
  }

  template <typename... Args>
  pqxx::result read(std::string&& q, Args... args) const {
    static auto conn = pqxx::connection(_dsn);
    if (!conn.is_open()) {
      conn = pqxx::connection(_dsn);
    }
    auto work = pqxx::work(conn);
    auto result = work.exec_params(q, args...);
    work.commit();
    return result;
  }

 private:
  std::string _host;
  int _port;
  std::string _user;
  std::string _password;
  std::string _database;
  std::string _dsn;
  const int _n = 8;

  mutable moodycamel::BlockingConcurrentQueue<
      std::function<void(pqxx::connection&)>>
      _writeQueue;

  mutable moodycamel::BlockingConcurrentQueue<pqxx::work> _readQueue;

  void _startWriteQueue();
  void _startReadQueue();
};

}  // namespace wss
