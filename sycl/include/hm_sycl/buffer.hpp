//
// Created by neo on 2025/10/25.
//

#ifndef SGL_BUFFER_H
#define SGL_BUFFER_H

#include "access.hpp"
#include "accessor.hpp"
#include "handler.hpp"
#include "range.hpp"

namespace hmsycl::sycl {
template <typename T, int Dimensions = 1> class buffer {
  T *_hostData;
  range<Dimensions> _range;

public:
  buffer(T *hostData, const range<Dimensions> &bufferRange) {}

  template <access::mode M> accessor<T,Dimensions> get_access(handler &h) {}
};
} // namespace hmsycl::sycl

#endif // SGL_BUFFER_H
