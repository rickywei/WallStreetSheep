#include <spdlog/spdlog.h>

#include "log.hpp"
#include "thread.hpp"
#include "trade.hpp"

int main(int argc, char *argv[]) {
  wss::replace_default_logger("ctp");
  for (int i = 0; i < argc; ++i) {
    SPDLOG_INFO("argv[{}]={}", i, argv[i]);
  }
  auto md = wss::Trade(std::string(argv[0]));
  md.start();

  wss::getGlobalThreadPool()->join();
}