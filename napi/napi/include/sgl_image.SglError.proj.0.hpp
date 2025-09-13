#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "taihe/common.hpp"
namespace sgl_image {
struct SglError;
}
namespace taihe {
template<>
struct as_abi<::sgl_image::SglError> {
    using type = int;
};
template<>
struct as_param<::sgl_image::SglError> {
    using type = ::sgl_image::SglError;
};
}
#pragma clang diagnostic pop
