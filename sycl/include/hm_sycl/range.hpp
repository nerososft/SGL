//
// Created by neo on 2025/10/25.
//

#ifndef SGL_RANGE_H
#define SGL_RANGE_H

namespace hmsycl::sycl {
template <int Dimensions = 1> class range {
  int size = 0;

public:
  range() = default;
  range(int size) {};
};
} // namespace hmsycl::sycl

#endif // SGL_RANGE_H
