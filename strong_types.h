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

#include <iostream>
#include <type_traits>
#include <utility>

// We aren't allowed to inherit from built-in types
//
// Inheritance is nice, but there are many types we can't inherit from that
// aren't just in-built types The alternative to inheritance is composition.

template <typename T, typename U, typename... Mixins>
class StrongType : Mixins... {
public:
  StrongType() = default;
  explicit StrongType(T value) : value_(value) {}
  StrongType(const StrongType&) = default;
  StrongType& operator=(const StrongType&) = default;
  StrongType(StrongType&&) = default;
  StrongType& operator=(StrongType&&) = default;

  // library-grade accessor set:
  // - expose reference views (so that working with the type is truly zero cost)
  // - const/non-const lvalue/rvalue
  // - const rvalue makes no semantic sense, so must define the following
  // triplet
  T& get() & { return value_; }
  const T& get() const& { return value_; }
  T&& get() && { return std::move(value_); }

private:
  T value_;
};

// NOTE: Mixins must have zero size to enable EBO on StrongType

struct Addable {
  template <typename T,
            typename = std::void_t<decltype(std::declval<T>().get() +
                                            std::declval<T>().get())>>
  friend T operator+(T a, T b) {
    return T{a.get() + b.get()};
  }
};

struct Subtractable {
  template <typename T>
  friend T operator-(T a, T b) {
    return T{a.get() - b.get()};
  }
};

struct Ostreamable {
  template <typename T>
  friend std::ostream& operator<<(std::ostream& out, T value) {
    out << value.get();
    return out;
  }
};
