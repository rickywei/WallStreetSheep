#include <spdlog/spdlog.h>
#include <yaml-cpp/yaml.h>

#include <iostream>
#include <memory>

#include "alphamaker/market/market.hpp"
#include "alphamaker/market/market_ctp.hpp"

int main() {
  YAML::Node config = YAML::LoadFile("../config.yaml");

  spdlog::info("read config {0}", YAML::Dump(config));

  am::MarketCtp *ctp = new am::MarketCtp(
      config["market_front_addr"], config["broker_id"], config["investor_id"],
      config["password"], config["flow_path"], config["is_using_udp"],
      config["is_multicast"]);
  ctp->init();

  return 0;
}