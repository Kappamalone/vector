#include "simple_logger.h"
#include "strong_types.h"
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

  ~ObjectPrinter() { LOG_DEBUG(id_, ": DTOR"); }

private:
  size_t id_;
};

/*
class Cat {
public:
  void speak() { std::cout << "meow\n"; }
};

using StrongCatVoid = StrongType<Cat, void>;
using StrongCatInt = StrongType<Cat, int>;

void cat_func(StrongCatVoid s) {
  s.speak();
}
*/

struct ATag {};
using A = StrongType<int, ATag, Addable>;
struct BTag {};
using B = StrongType<int, BTag, Addable>;

void arithmetic_with_a(A first, A second) {
  std::cout << first + second << "\n";
}

int main() {
  /*
  Vector<ObjectPrinter> victim;
  victim.push_back(ObjectPrinter{});
  victim.push_back(bjectPrinter{});

  int* ptr = nullptr;
  std::cout << "about to crash..." << std::endl;
  *ptr = 42;
  */

  std::cout << "hello world\n";

  // cat_func(StrongCatVoid{});
  // cat_func(StrongCatInt{}); // doesn't compile

  arithmetic_with_a(A{1}, A{2});


}
