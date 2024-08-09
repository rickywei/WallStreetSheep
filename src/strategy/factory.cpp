#include "Example.hpp"

namespace wss {

extern "C" {
__attribute__((visibility("default"))) std::unique_ptr<IStrategy> create(
    std::shared_ptr<ICtx> ctx, std::string name) {
  if (name == "example")
    return std::make_unique<Example>(ctx, name);
  else
    return nullptr;
}
}

}  // namespace wss
