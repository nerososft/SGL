//
// Created by neo on 2025/10/25.
//

#ifndef SGL_RANGE_H
#define SGL_RANGE_H

namespace hmsycl::sycl {
template <int Dimensions = 1> class range {
public:
  range() = default;
  range(size_t dim1) {};
  range(size_t dim1, size_t dim2) {};
};
} // namespace hmsycl::sycl

#endif // SGL_RANGE_H
