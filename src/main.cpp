#include "WallStreetSheep/common/log.hpp"
#include "WallStreetSheep/manager/Manager.hpp"

int main() {
  wss::replace_default_logger();

  wss::Manager("../config.yaml").start();

  return 0;
}