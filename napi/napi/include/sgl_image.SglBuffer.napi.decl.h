#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "taihe/napi_runtime.hpp"
#include "sgl_image.SglBuffer.proj.1.hpp"
::sgl_image::SglBuffer sgl_image_SglBuffer_fromNAPI0(napi_env env, napi_value napi_obj);
napi_value sgl_image_SglBuffer_intoNAPI0(napi_env env, ::sgl_image::SglBuffer const& cpp_obj);
#pragma clang diagnostic pop
