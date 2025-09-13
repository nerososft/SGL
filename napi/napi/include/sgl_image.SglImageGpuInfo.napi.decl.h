#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "taihe/napi_runtime.hpp"
#include "sgl_image.SglImageGpuInfo.proj.1.hpp"
::sgl_image::SglImageGpuInfo sgl_image_SglImageGpuInfo_fromNAPI0(napi_env env, napi_value napi_obj);
napi_value sgl_image_SglImageGpuInfo_intoNAPI0(napi_env env, ::sgl_image::SglImageGpuInfo const& cpp_obj);
#pragma clang diagnostic pop
