#pragma once

#include <boost/asio/thread_pool.hpp>
#include <functional>
#include <memory>

namespace am {

std::shared_ptr<boost::asio::thread_pool> getGlobalThreadPool();

void postTask(std::function<void()> &&fn);

}  // namespace am
