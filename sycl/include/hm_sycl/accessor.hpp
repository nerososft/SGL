//
// Created by neo on 2025/10/25.
//

#ifndef SGL_ACCESSOR_H
#define SGL_ACCESSOR_H

#include "id.hpp"

namespace hmsycl::sycl {
template <typename T, int Dimensions> class accessor {
  using reference = T &;

public:
  reference operator[](id<Dimensions> index) const {}
};
} // namespace hmsycl::sycl
#endif // SGL_ACCESSOR_H
