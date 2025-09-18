#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "sgl_image.Info.napi.decl.h"
#include "sgl_image.Info.proj.2.hpp"
#include "sgl_image.SglImageCpuInfo.napi.impl.h"
#include "sgl_image.SglImageGpuInfo.napi.impl.h"
inline ::sgl_image::Info sgl_image_Info_fromNAPI0(napi_env env, napi_value napi_obj) {
    napi_valuetype value_ty;
    NAPI_CALL(env, napi_typeof(env, napi_obj, &value_ty));
    bool flag;
}
inline napi_value sgl_image_Info_intoNAPI0(napi_env env, ::sgl_image::Info const& cpp_value) {
    napi_value napi_obj = nullptr;
    switch (cpp_value.get_tag()) {
    case ::sgl_image::Info::tag_t::gpu: {
        napi_value napi_obj_field = sgl_image_SglImageGpuInfo_intoNAPI0(env, cpp_value.get_gpu_ref());
        napi_obj = napi_obj_field;
        break;
    }
    case ::sgl_image::Info::tag_t::cpu: {
        napi_value napi_obj_field = sgl_image_SglImageCpuInfo_intoNAPI0(env, cpp_value.get_cpu_ref());
        napi_obj = napi_obj_field;
        break;
    }
    }
    return napi_obj;
}
#pragma clang diagnostic pop
