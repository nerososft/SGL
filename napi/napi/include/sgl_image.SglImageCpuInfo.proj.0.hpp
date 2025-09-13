#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "taihe/common.hpp"
#include "sgl_image.SglImageCpuInfo.abi.0.h"
namespace sgl_image {
struct SglImageCpuInfo;
}
namespace taihe {
template<>
struct as_abi<::sgl_image::SglImageCpuInfo> {
    using type = struct sgl_image_SglImageCpuInfo_t0;
};
template<>
struct as_abi<::sgl_image::SglImageCpuInfo const&> {
    using type = struct sgl_image_SglImageCpuInfo_t0 const*;
};
template<>
struct as_param<::sgl_image::SglImageCpuInfo> {
    using type = ::sgl_image::SglImageCpuInfo const&;
};
}
#pragma clang diagnostic pop
