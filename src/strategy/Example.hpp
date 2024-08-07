#include "IStrategy.hpp"

namespace wss {

class Example : public IStrategy {
 public:
  Example(std::string name);
};

}  // namespace wss