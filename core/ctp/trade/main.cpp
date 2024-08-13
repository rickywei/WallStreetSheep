#include <nlohmann/json.hpp>

#include "../../common/log.hpp"
#include "../../common/thread.hpp"
#include "../trade/Trade.hpp"

int main(int argc, char *argv[]) {
  wss::replace_default_logger("ctpTrade");
  auto md = wss::Trade();
  nlohmann::json j;
  

  wss::getGlobalThreadPool()->join();
}