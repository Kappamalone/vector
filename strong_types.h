#pragma once

// what are "strong types"?
//
// essentially, they are ways of utilising the type system to remove entire
// classes of bugs entirely. concretely, they have some additional information
// (for example, a tag struct) that the compiler can then use to reject invalid
// code
//
// for example, consider the following snippet:
//
// using A = float;
// using B = float;
// void do_something(A a, B b) { ... }
//
// There is nothing stopping a misinformed developer from doing the following:
// A a = ...;
// B b = ...;
// do_something(b, a);

// requirements:
// 1) a way to access the underlying type
// 2) mixins for common behaviour
// 3) forwarding special member functions correctly for ergonomic usage

#include <type_traits>
#include <utility>

// We aren't allowed to inherit from built-in types
//
// Inheritance is nice, but there are many types we can't inherit from that
// aren't just in-built types The alternative to inheritance is composition.

struct Addable {
  template <typename T>
  friend decltype(std::declval<T>().get()) operator+(T a, T b) {
    return a.get() + b.get();
  }
};

template <typename T, typename U, typename Mixin>
class StrongType : Mixin {
public:
  StrongType() = default;
  StrongType(T value) : value_(value) {}

  T get() { return value_; }

private:
  T value_;
};
