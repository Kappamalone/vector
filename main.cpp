#include "simple_logger.h"
#include "vector.h"
#include <iostream>

class ObjectPrinter {
public:
  ObjectPrinter() { LOG_DEBUG("OBJECT CTOR"); }

  ObjectPrinter(ObjectPrinter& other) { LOG_DEBUG("COPY CTOR"); }

  ObjectPrinter& operator=(ObjectPrinter& other) {
    LOG_DEBUG("COPY ASSIGNMENT");
    return *this;
  }

  ObjectPrinter(ObjectPrinter&& other) noexcept { LOG_DEBUG("MOVE CTOR"); }

  ObjectPrinter& operator=(ObjectPrinter&& other) noexcept {
    LOG_DEBUG("MOVE ASSIGNMENT");
    return *this;
  }
};

int main() {
  std::cout << "hello world\n";
  Vector<ObjectPrinter> victim;
  victim.push_back(ObjectPrinter{});
}
