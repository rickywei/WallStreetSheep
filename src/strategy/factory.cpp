#include "Example.hpp"
#include "IStrategy.hpp"

namespace wss {

extern "C" {
__attribute__((visibility("default"))) std::unique_ptr<IStrategy> create(
    std::string name) {
  if (name == "xxx")
    return std::make_unique<Example>(name);
  else
    return nullptr;
}
}

}  // namespace wss
