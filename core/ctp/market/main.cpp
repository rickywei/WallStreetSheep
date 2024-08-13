#include "../../common/log.hpp"
#include "../../common/thread.hpp"
#include "../market/Market.hpp"

int main() {
  wss::replace_default_logger("ctpMarket");
  auto md = wss::Market();

  wss::getGlobalThreadPool()->join();
}