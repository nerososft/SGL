//
// Created by neo on 2025/10/25.
//

#ifndef SGL_QUEUE_H
#define SGL_QUEUE_H

#include "event.hpp"

namespace hmsycl::sycl {
class queue {

public:
  template <typename T> event submit(T cgf) {}
};
} // namespace hmsycl::sycl
#endif // SGL_QUEUE_H
