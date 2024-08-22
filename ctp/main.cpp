#include <spdlog/spdlog.h>

#include <nlohmann/json.hpp>

#include "log.hpp"
#include "market.hpp"
#include "thread.hpp"
#include "trade.hpp"

int main(int argc, char *argv[]) {
  wss::replace_default_logger("ctp");
  for (int i = 0; i < argc; ++i) {
    SPDLOG_INFO("argv[{}]={}", i, argv[i]);
  }
  auto conf = std::string(argv[0]);
  auto j = nlohmann::json::parse(conf);

  auto td =
      j.at("trade").get<bool>() ? std::make_unique<wss::Trade>(conf) : nullptr;
  if (td != nullptr) td->start();

  auto md = j.at("market").get<bool>() ? std::make_unique<wss::Market>(conf)
                                       : nullptr;
  if (md != nullptr) md->start();

  wss::getGlobalThreadPool()->join();
}