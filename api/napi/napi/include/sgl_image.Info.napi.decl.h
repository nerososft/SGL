#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "sgl_image.Info.proj.1.hpp"
::sgl_image::Info sgl_image_Info_fromNAPI0(napi_env env, napi_value napi_obj);
napi_value sgl_image_Info_intoNAPI0(napi_env env, ::sgl_image::Info const& cpp_obj);
#pragma clang diagnostic pop
