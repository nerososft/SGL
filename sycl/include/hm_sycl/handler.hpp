//
// Created by neo on 2025/10/25.
//

#ifndef SGL_HANDLER_H
#define SGL_HANDLER_H

#include "range.hpp"

namespace hmsycl::sycl {
class handler {

public:
  template <int Dimensions = 1, typename... ReductionsAndKernel>
  void parallel_for(range<Dimensions> r, const ReductionsAndKernel &...kernel) {
  }
};
} // namespace hmsycl::sycl
#endif // SGL_HANDLER_H
