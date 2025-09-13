#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "taihe/common.hpp"
#include "sgl_image.SglImageGpuInfo.abi.0.h"
namespace sgl_image {
struct SglImageGpuInfo;
}
namespace taihe {
template<>
struct as_abi<::sgl_image::SglImageGpuInfo> {
    using type = struct sgl_image_SglImageGpuInfo_t0;
};
template<>
struct as_abi<::sgl_image::SglImageGpuInfo const&> {
    using type = struct sgl_image_SglImageGpuInfo_t0 const*;
};
template<>
struct as_param<::sgl_image::SglImageGpuInfo> {
    using type = ::sgl_image::SglImageGpuInfo const&;
};
}
#pragma clang diagnostic pop
