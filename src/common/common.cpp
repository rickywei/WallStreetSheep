#include "WallStreetSheep/common/common.hpp"

#include <boost/asio/post.hpp>

namespace wss {

std::shared_ptr<boost::asio::thread_pool> getGlobalThreadPool() {
  static std::shared_ptr<boost::asio::thread_pool> pool =
      std::make_shared<boost::asio::thread_pool>();
  return pool;
}
void postTask(std::function<void()> &&fn) {
  boost::asio::post(*getGlobalThreadPool(), std::move(fn));
}

}  // namespace wss
