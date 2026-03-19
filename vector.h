#pragma once
#include "simple_logger.h"
#include <memory>

// notes:
// need to think about alignment of memoroy

template <typename T>
class Vector {
public:
  Vector()
      : size_(1), memory_(static_cast<T*>(new char[sizeof(T) * size_])),
        length_(0) {
    LOG_INFO("size of T: ", sizeof(T));
  }

  void push_back(T value) {
    if (length_ < size_) {
      LOG_INFO("length < size");

      new ((void*)(size_ + (length_ * sizeof(T)))) T(value);
      ++length_;
      return;
    }

    LOG_INFO("resizing vector");
    // we now have to move our elements from the old memory to new memory
    // a standard compliant vector must guarantee strong exception safety, therefore
    // we:
    // 1) use the move ctor if it is non throwing or the copy ctor throws (the latter case means we can't uphold exception safety)
  }

private:
  size_t size_;
  T* memory_;
  size_t length_;
};
