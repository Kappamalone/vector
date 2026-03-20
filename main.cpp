#include "simple_logger.h"
#include "vector.h"
#include <iostream>

class ObjectPrinter {
public:
  inline static size_t id_counter_ = 1;

  ObjectPrinter() : id_(id_counter_++) { LOG_DEBUG(id_, ": OBJECT CTOR"); }

  ObjectPrinter(const ObjectPrinter& other) : id_{other.id_ * 100} { LOG_DEBUG(id_, ": COPY CTOR"); }

  ObjectPrinter& operator=(const ObjectPrinter& other) {
    id_ = other.id_ * 100;
    LOG_DEBUG(id_, ": COPY ASSIGNMENT");
    return *this;
  }

  ObjectPrinter(ObjectPrinter&& other) : id_{std::exchange(other.id_, 0)} {
    LOG_DEBUG(id_, ": MOVE CTOR");
  }

  ObjectPrinter& operator=(ObjectPrinter&& other) noexcept {
    id_ = other.id_;
    other.id_ = 0;
    LOG_DEBUG(id_, ": MOVE ASSIGNMENT");
    return *this;
  }

  ~ObjectPrinter() {
    LOG_DEBUG(id_, ": DTOR");
  }

private:
  size_t id_;
};

int main() {
  std::cout << "hello world\n";
  Vector<ObjectPrinter> victim;
  victim.push_back(ObjectPrinter{});
  victim.push_back(ObjectPrinter{});
}
