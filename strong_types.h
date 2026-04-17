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

template <typename... Ts>
struct PreCpp17MixinsAreEmpty : std::true_type {
};

template <typename T, typename... Ts>
struct PreCpp17MixinsAreEmpty<T, Ts...> {
  static constexpr bool value = std::is_empty_v<T> && PreCpp17MixinsAreEmpty<Ts...>::value;
};

template <typename T, typename U, typename... Mixins>
class StrongType : Mixins... {
  // static_assert((std::is_empty_v<Mixins> && ...));
  static_assert(PreCpp17MixinsAreEmpty<Mixins...>::value);

public:
  StrongType() = delete;
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
  constexpr T& get() & { return value_; }
  constexpr const T& get() const& { return value_; }
  constexpr T&& get() && { return std::move(value_); }

private:
  T value_;
};

// NOTE: Mixins must have zero size to enable EBO on StrongType

// little macro lesson:
// macros do not concatenate tokens unless you explicitly ask, which is where
// the token pasting operator(##) is useful

#define OPERATOR_MIXIN(NAME, OP)                                               \
  struct NAME {                                                                \
    template <typename T,                                                      \
              typename = std::void_t<decltype(T{                               \
                  std::declval<T>().get() OP std::declval<T>().get()})>>       \
    friend T operator OP(const T & a, const T & b) {                          \
      return T{a.get() OP b.get()};                                            \
    }                                                                          \
  };

OPERATOR_MIXIN(Addable, +);
OPERATOR_MIXIN(Subtractable, -);

struct Ostreamable {
  template <typename T>
  friend std::ostream& operator<<(std::ostream& out, T value) {
    out << value.get();
    return out;
  }
};
