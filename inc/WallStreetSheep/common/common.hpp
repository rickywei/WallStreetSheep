#pragma once

#include <boost/asio/thread_pool.hpp>
#include <functional>
#include <memory>

namespace wss {

std::shared_ptr<boost::asio::thread_pool> getGlobalThreadPool();

void postTask(std::function<void()> &&fn);

void replace_default_logger();

}  // namespace wss
