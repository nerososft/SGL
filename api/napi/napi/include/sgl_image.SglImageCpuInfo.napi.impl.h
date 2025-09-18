#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "sgl_image.SglImageCpuInfo.napi.decl.h"
#include "sgl_image.SglImageCpuInfo.proj.2.hpp"
#pragma clang diagnostic ignored "-Wmissing-braces"
static napi_value sgl_image_SglImageCpuInfo_width_getter0(napi_env env, napi_callback_info info) {
    napi_value thisobj;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisobj, nullptr));
    ::sgl_image::SglImageCpuInfo* cpp_ptr;
    NAPI_CALL(env, napi_unwrap(env, thisobj, reinterpret_cast<void **>(&cpp_ptr)));
    napi_value napi_field_result = nullptr;
    NAPI_CALL(env, napi_create_uint32(env, cpp_ptr->width, &napi_field_result));
    return napi_field_result;
}
static napi_value sgl_image_SglImageCpuInfo_width_setter0(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    napi_value thisobj;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisobj, nullptr));
    ::sgl_image::SglImageCpuInfo* cpp_ptr;
    NAPI_CALL(env, napi_unwrap(env, thisobj, reinterpret_cast<void **>(&cpp_ptr)));
    uint32_t cpp_field_result_tmp;
    NAPI_CALL(env, napi_get_value_uint32(env, args[0], &cpp_field_result_tmp));
    uint32_t cpp_field_result = cpp_field_result_tmp;
    cpp_ptr->width = cpp_field_result;
    return nullptr;
}
static napi_value sgl_image_SglImageCpuInfo_height_getter0(napi_env env, napi_callback_info info) {
    napi_value thisobj;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisobj, nullptr));
    ::sgl_image::SglImageCpuInfo* cpp_ptr;
    NAPI_CALL(env, napi_unwrap(env, thisobj, reinterpret_cast<void **>(&cpp_ptr)));
    napi_value napi_field_result = nullptr;
    NAPI_CALL(env, napi_create_uint32(env, cpp_ptr->height, &napi_field_result));
    return napi_field_result;
}
static napi_value sgl_image_SglImageCpuInfo_height_setter0(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    napi_value thisobj;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisobj, nullptr));
    ::sgl_image::SglImageCpuInfo* cpp_ptr;
    NAPI_CALL(env, napi_unwrap(env, thisobj, reinterpret_cast<void **>(&cpp_ptr)));
    uint32_t cpp_field_result_tmp;
    NAPI_CALL(env, napi_get_value_uint32(env, args[0], &cpp_field_result_tmp));
    uint32_t cpp_field_result = cpp_field_result_tmp;
    cpp_ptr->height = cpp_field_result;
    return nullptr;
}
static napi_value sgl_image_SglImageCpuInfo_channels_getter0(napi_env env, napi_callback_info info) {
    napi_value thisobj;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisobj, nullptr));
    ::sgl_image::SglImageCpuInfo* cpp_ptr;
    NAPI_CALL(env, napi_unwrap(env, thisobj, reinterpret_cast<void **>(&cpp_ptr)));
    napi_value napi_field_result = nullptr;
    NAPI_CALL(env, napi_create_uint32(env, cpp_ptr->channels, &napi_field_result));
    return napi_field_result;
}
static napi_value sgl_image_SglImageCpuInfo_channels_setter0(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    napi_value thisobj;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisobj, nullptr));
    ::sgl_image::SglImageCpuInfo* cpp_ptr;
    NAPI_CALL(env, napi_unwrap(env, thisobj, reinterpret_cast<void **>(&cpp_ptr)));
    uint32_t cpp_field_result_tmp;
    NAPI_CALL(env, napi_get_value_uint32(env, args[0], &cpp_field_result_tmp));
    uint32_t cpp_field_result = cpp_field_result_tmp;
    cpp_ptr->channels = cpp_field_result;
    return nullptr;
}
static napi_value sgl_image_SglImageCpuInfo_bytesPerLine_getter0(napi_env env, napi_callback_info info) {
    napi_value thisobj;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisobj, nullptr));
    ::sgl_image::SglImageCpuInfo* cpp_ptr;
    NAPI_CALL(env, napi_unwrap(env, thisobj, reinterpret_cast<void **>(&cpp_ptr)));
    napi_value napi_field_result = nullptr;
    NAPI_CALL(env, napi_create_uint32(env, cpp_ptr->bytesPerLine, &napi_field_result));
    return napi_field_result;
}
static napi_value sgl_image_SglImageCpuInfo_bytesPerLine_setter0(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    napi_value thisobj;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisobj, nullptr));
    ::sgl_image::SglImageCpuInfo* cpp_ptr;
    NAPI_CALL(env, napi_unwrap(env, thisobj, reinterpret_cast<void **>(&cpp_ptr)));
    uint32_t cpp_field_result_tmp;
    NAPI_CALL(env, napi_get_value_uint32(env, args[0], &cpp_field_result_tmp));
    uint32_t cpp_field_result = cpp_field_result_tmp;
    cpp_ptr->bytesPerLine = cpp_field_result;
    return nullptr;
}
static napi_value sgl_image_SglImageCpuInfo_data_ptr_getter03(napi_env env, napi_callback_info info) {
    napi_value thisobj;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisobj, nullptr));
    ::sgl_image::SglImageCpuInfo* cpp_ptr;
    NAPI_CALL(env, napi_unwrap(env, thisobj, reinterpret_cast<void **>(&cpp_ptr)));
    napi_value napi_field_result = nullptr;
    NAPI_CALL(env, None(env, cpp_ptr->data_ptr, &napi_field_result));
    return napi_field_result;
}
static napi_value sgl_image_SglImageCpuInfo_data_ptr_setter03(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    napi_value thisobj;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisobj, nullptr));
    ::sgl_image::SglImageCpuInfo* cpp_ptr;
    NAPI_CALL(env, napi_unwrap(env, thisobj, reinterpret_cast<void **>(&cpp_ptr)));
    uint64_t cpp_field_result_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, args[0], &cpp_field_result_tmp));
    uint64_t cpp_field_result = cpp_field_result_tmp;
    cpp_ptr->data_ptr = cpp_field_result;
    return nullptr;
}
inline napi_value sgl_image_SglImageCpuInfo_constructor0_inner(napi_env env, napi_callback_info info) {
    napi_status _status;
    napi_value thisobj;
    size_t argc = 5;
    napi_value args[5];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisobj, nullptr));
    uint32_t cpp_field_width_tmp;
    NAPI_CALL(env, napi_get_value_uint32(env, args[0], &cpp_field_width_tmp));
    uint32_t cpp_field_width = cpp_field_width_tmp;
    uint32_t cpp_field_height_tmp;
    NAPI_CALL(env, napi_get_value_uint32(env, args[1], &cpp_field_height_tmp));
    uint32_t cpp_field_height = cpp_field_height_tmp;
    uint32_t cpp_field_channels_tmp;
    NAPI_CALL(env, napi_get_value_uint32(env, args[2], &cpp_field_channels_tmp));
    uint32_t cpp_field_channels = cpp_field_channels_tmp;
    uint32_t cpp_field_bytesPerLine_tmp;
    NAPI_CALL(env, napi_get_value_uint32(env, args[3], &cpp_field_bytesPerLine_tmp));
    uint32_t cpp_field_bytesPerLine = cpp_field_bytesPerLine_tmp;
    uint64_t cpp_field_data_ptr_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, args[4], &cpp_field_data_ptr_tmp));
    uint64_t cpp_field_data_ptr = cpp_field_data_ptr_tmp;
    ::sgl_image::SglImageCpuInfo* cpp_ptr = new ::sgl_image::SglImageCpuInfo{std::move(cpp_field_width), std::move(cpp_field_height), std::move(cpp_field_channels), std::move(cpp_field_bytesPerLine), std::move(cpp_field_data_ptr)};
    _status = napi_wrap(env, thisobj, cpp_ptr, []([[maybe_unused]] napi_env env, void* finalize_data, [[maybe_unused]] void* finalize_hint) {
        delete static_cast<::sgl_image::SglImageCpuInfo*>(finalize_data);
    }, nullptr, nullptr);
    if (_status != napi_ok) {
        delete thisobj;
        napi_throw_error(env,
            "ERR_WRAP_FAILED",
            ("Native object wrapping failed (status " + std::to_string(_status) + ")").c_str()
        );
        return nullptr;
    }
    return thisobj;
}
inline napi_ref& ctor_ref_sgl_image_SglImageCpuInfo_t0_inner() {
    static napi_ref instance = nullptr;
    return instance;
}
inline ::sgl_image::SglImageCpuInfo sgl_image_SglImageCpuInfo_fromNAPI0(napi_env env, napi_value napi_obj) {
    napi_value napi_field_width = nullptr;
    NAPI_CALL(env, napi_get_named_property(env, napi_obj, "width", &napi_field_width));
    uint32_t cpp_field_width_tmp;
    NAPI_CALL(env, napi_get_value_uint32(env, napi_field_width, &cpp_field_width_tmp));
    uint32_t cpp_field_width = cpp_field_width_tmp;
    napi_value napi_field_height = nullptr;
    NAPI_CALL(env, napi_get_named_property(env, napi_obj, "height", &napi_field_height));
    uint32_t cpp_field_height_tmp;
    NAPI_CALL(env, napi_get_value_uint32(env, napi_field_height, &cpp_field_height_tmp));
    uint32_t cpp_field_height = cpp_field_height_tmp;
    napi_value napi_field_channels = nullptr;
    NAPI_CALL(env, napi_get_named_property(env, napi_obj, "channels", &napi_field_channels));
    uint32_t cpp_field_channels_tmp;
    NAPI_CALL(env, napi_get_value_uint32(env, napi_field_channels, &cpp_field_channels_tmp));
    uint32_t cpp_field_channels = cpp_field_channels_tmp;
    napi_value napi_field_bytesPerLine = nullptr;
    NAPI_CALL(env, napi_get_named_property(env, napi_obj, "bytesPerLine", &napi_field_bytesPerLine));
    uint32_t cpp_field_bytesPerLine_tmp;
    NAPI_CALL(env, napi_get_value_uint32(env, napi_field_bytesPerLine, &cpp_field_bytesPerLine_tmp));
    uint32_t cpp_field_bytesPerLine = cpp_field_bytesPerLine_tmp;
    napi_value napi_field_data_ptr = nullptr;
    NAPI_CALL(env, napi_get_named_property(env, napi_obj, "data_ptr", &napi_field_data_ptr));
    uint64_t cpp_field_data_ptr_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, napi_field_data_ptr, &cpp_field_data_ptr_tmp));
    uint64_t cpp_field_data_ptr = cpp_field_data_ptr_tmp;
    return ::sgl_image::SglImageCpuInfo{std::move(cpp_field_width), std::move(cpp_field_height), std::move(cpp_field_channels), std::move(cpp_field_bytesPerLine), std::move(cpp_field_data_ptr)};
}
inline napi_value sgl_image_SglImageCpuInfo_intoNAPI0(napi_env env, ::sgl_image::SglImageCpuInfo const& cpp_obj) {
    napi_value napi_field_width = nullptr;
    NAPI_CALL(env, napi_create_uint32(env, cpp_obj.width, &napi_field_width));
    napi_value napi_field_height = nullptr;
    NAPI_CALL(env, napi_create_uint32(env, cpp_obj.height, &napi_field_height));
    napi_value napi_field_channels = nullptr;
    NAPI_CALL(env, napi_create_uint32(env, cpp_obj.channels, &napi_field_channels));
    napi_value napi_field_bytesPerLine = nullptr;
    NAPI_CALL(env, napi_create_uint32(env, cpp_obj.bytesPerLine, &napi_field_bytesPerLine));
    napi_value napi_field_data_ptr = nullptr;
    NAPI_CALL(env, None(env, cpp_obj.data_ptr, &napi_field_data_ptr));
    napi_value args[5] = {napi_field_width, napi_field_height, napi_field_channels, napi_field_bytesPerLine, napi_field_data_ptr};
    napi_value napi_obj = nullptr, constructor = nullptr;
    NAPI_CALL(env, napi_get_reference_value(env, ctor_ref_sgl_image_SglImageCpuInfo_t0_inner(), &constructor));
    NAPI_CALL(env, napi_new_instance(env, constructor, 5, args, &napi_obj));
    return napi_obj;
}
#pragma clang diagnostic pop
