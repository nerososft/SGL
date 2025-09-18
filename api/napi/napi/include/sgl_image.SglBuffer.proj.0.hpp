#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "sgl_image.SglBuffer.abi.0.h"
#include "taihe/common.hpp"
namespace sgl_image {
struct SglBuffer;
}
namespace taihe {
template<>
struct as_abi<::sgl_image::SglBuffer> {
    using type = struct sgl_image_SglBuffer_t0;
};
template<>
struct as_abi<::sgl_image::SglBuffer const&> {
    using type = struct sgl_image_SglBuffer_t0 const*;
};
template<>
struct as_param<::sgl_image::SglBuffer> {
    using type = ::sgl_image::SglBuffer const&;
};
}
#pragma clang diagnostic pop
