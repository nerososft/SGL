#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "sgl_image.SglImageInfo.proj.1.hpp"
#include "taihe/napi_runtime.hpp"
::sgl_image::SglImageInfo sgl_image_SglImageInfo_fromNAPI0(napi_env env, napi_value napi_obj);
napi_value sgl_image_SglImageInfo_intoNAPI0(napi_env env, ::sgl_image::SglImageInfo const& cpp_obj);
#pragma clang diagnostic pop
