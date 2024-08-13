#include "../../../common/thread.hpp"
#include "../IStrategy.hpp"
#include "Example.hpp"

int main(int argc, char* argv[]) {
  auto s = std::make_unique<wss::Example>("Example");
  s->subscribe();

  wss::getGlobalThreadPool()->join();
}