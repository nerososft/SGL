#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "sgl_image.Info.abi.0.h"
#include "taihe/common.hpp"
namespace sgl_image {
struct Info;
}
namespace taihe {
template<>
struct as_abi<::sgl_image::Info> {
    using type = struct sgl_image_Info_t0;
};
template<>
struct as_abi<::sgl_image::Info const&> {
    using type = struct sgl_image_Info_t0 const*;
};
template<>
struct as_param<::sgl_image::Info> {
    using type = ::sgl_image::Info const&;
};
}
#pragma clang diagnostic pop
