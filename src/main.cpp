#include "WallStreetSheep/common/common.hpp"
#include "WallStreetSheep/manager/ManagerCtp.hpp"

int main() {
  wss::replace_default_logger();

  wss::ManagerCtp("../config.yaml").start();

  return 0;
}