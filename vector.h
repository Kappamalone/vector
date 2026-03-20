#pragma once

#include "simple_logger.h"
#include <memory>
#include <type_traits>

// NOTES during:
// - need to think about alignment of memory for a type T
// - new operator DIFFERENT to operator new

template <typename Fn>
class ScopeGuard {
public:
  ScopeGuard(Fn&& fn) : fn_{std::move(fn)}, active_{true} {}
  ScopeGuard() = delete;
  ScopeGuard(const ScopeGuard& other) = delete;
  ScopeGuard(ScopeGuard&& other) = delete;
  ScopeGuard& operator=(const ScopeGuard& other) = delete;
  ScopeGuard& operator=(ScopeGuard&& other) = delete;

  // idk how to better model this
  void deactivate() { active_ = false; }

  ~ScopeGuard() {
    if (active_) {
      fn_();
    }
  }

private:
  Fn fn_;
  bool active_;
};

template <typename T>
class Vector {
public:
  Vector()
      : size_(1), memory_(static_cast<T*>(::operator new(sizeof(T) * size_))),
        length_(0) {
    LOG_INFO("size of T: ", sizeof(T));
  }

  void push_back(T value) {
    LOG_INFO("START PUSH_BACK ========");

    if (length_ < size_) {
      LOG_INFO("length < size");

      ::new (memory_ + length_) T(std::move(value));
      ++length_;
      return;
    }

    LOG_INFO("resizing vector");

    // is there a name for like, doing something first then committing it?

    // allocate new memory + use a guard to free it if a subsequent operation
    // throws
    size_t new_size = size_ * 2;
    T* new_memory = static_cast<T*>(::operator new(sizeof(T) * new_size));
    // is this CTAD? yes!
    ScopeGuard guard{[&]() { ::operator delete(new_memory); }};

    if (std::is_nothrow_move_constructible_v<T> || !std::is_copy_constructible_v<T>) {
      LOG_INFO("move ctor path");

      // [0, size_t) = old elements
      for (size_t i = 0; i < size_; ++i) {
        ::new (new_memory + i) T(std::move(memory_[i]));
      }
      ::new (new_memory + size_) T(std::move(value));
    } else {
      LOG_INFO("copy ctor path");

      ::new (new_memory + size_) T(value);
      ScopeGuard guard1{[&]() { new_memory[size_].~T(); }};

      // unroll in the event of an exception
      size_t num_elements_constructed = 0;
      ScopeGuard guard2{[&]() {
        for (size_t i = 0; i < num_elements_constructed; ++i) {
          new_memory[i].~T();
        }
      }};

      for (size_t i = 0; i < size_; ++i) {
        ::new (new_memory + i) T(memory_[i]);
        ++num_elements_constructed;
      }


      guard1.deactivate();
      guard2.deactivate();
    }

    // if all is good, cleanup old memory and commit changes to vector
    for (size_t i = 0; i < size_; ++i) {
      memory_[i].~T();
    }
    std::swap(new_memory, memory_);
    size_ = new_size;
    ++length_;

    LOG_INFO("END   PUSH_BACK ========");
    return;
  }

private:
  size_t size_;
  T* memory_;
  size_t length_;
};


// NOTES after:
//
// Actual implementations have nice utility functions that let them enjoy life
// For example:
// - std::uninitialized_move
// - std::uninitialized_copy (which is exception safe!) + internally a move if no except iterator
//  - as a side note, i don't know too much about move iterators
// - compile time code path selection (SFINAE) + specific handling of trivial types
// - move is also chosen if copy isn't available
// - also, the guard stuff is cute but exceptions need to rethrown to the caller, meaning you need explicit try-catch blocks
