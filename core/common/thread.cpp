#include <spdlog/spdlog.h>

#include <boost/asio/post.hpp>
#include <boost/asio/thread_pool.hpp>
#include <functional>
#include <memory>

namespace wss {

std::shared_ptr<boost::asio::thread_pool> getGlobalThreadPool() {
  static std::shared_ptr<boost::asio::thread_pool> pool =
      std::make_shared<boost::asio::thread_pool>();
  return pool;
}

void postTask(std::function<void()> &&fn) {
  boost::asio::post(*getGlobalThreadPool(), [f = std::move(fn)]() {
    try {
      f();
    } catch (std::exception &e) {
      SPDLOG_ERROR("task exec failed, exception={}", e.what());
    }
  });
}

}  // namespace wss
