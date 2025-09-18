#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "sgl_image.SglImageCpuInfo.proj.1.hpp"
#include "taihe/napi_runtime.hpp"
::sgl_image::SglImageCpuInfo sgl_image_SglImageCpuInfo_fromNAPI0(napi_env env, napi_value napi_obj);
napi_value sgl_image_SglImageCpuInfo_intoNAPI0(napi_env env, ::sgl_image::SglImageCpuInfo const& cpp_obj);
#pragma clang diagnostic pop
