#include "common/log.hpp"
#include "manager/Manager.hpp"

int main() {
  wss::replace_default_logger();

  wss::Manager("./config.yaml").start();

  return 0;
}