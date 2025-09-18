#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "taihe/common.hpp"
namespace sgl_image {
struct SglBufferType;
}
namespace taihe {
template<>
struct as_abi<::sgl_image::SglBufferType> {
    using type = int;
};
template<>
struct as_param<::sgl_image::SglBufferType> {
    using type = ::sgl_image::SglBufferType;
};
}
#pragma clang diagnostic pop
