#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "sgl_image.SglImageInfo.abi.0.h"
#include "taihe/common.hpp"
namespace sgl_image {
struct SglImageInfo;
}
namespace taihe {
template<>
struct as_abi<::sgl_image::SglImageInfo> {
    using type = struct sgl_image_SglImageInfo_t0;
};
template<>
struct as_abi<::sgl_image::SglImageInfo const&> {
    using type = struct sgl_image_SglImageInfo_t0 const*;
};
template<>
struct as_param<::sgl_image::SglImageInfo> {
    using type = ::sgl_image::SglImageInfo const&;
};
}
#pragma clang diagnostic pop
