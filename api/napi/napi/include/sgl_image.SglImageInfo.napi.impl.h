#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "sgl_image.Info.napi.impl.h"
#include "sgl_image.SglImageInfo.napi.decl.h"
#include "sgl_image.SglImageInfo.proj.2.hpp"
#pragma clang diagnostic ignored "-Wmissing-braces"
static napi_value sgl_image_SglImageInfo_type_getter0(napi_env env, napi_callback_info info) {
    napi_value thisobj;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisobj, nullptr));
    ::sgl_image::SglImageInfo* cpp_ptr;
    NAPI_CALL(env, napi_unwrap(env, thisobj, reinterpret_cast<void **>(&cpp_ptr)));
    napi_value napi_field_result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(cpp_ptr->type.get_value())), &napi_field_result));
    return napi_field_result;
}
static napi_value sgl_image_SglImageInfo_type_setter0(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    napi_value thisobj;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisobj, nullptr));
    ::sgl_image::SglImageInfo* cpp_ptr;
    NAPI_CALL(env, napi_unwrap(env, thisobj, reinterpret_cast<void **>(&cpp_ptr)));
    int32_t cpp_field_result_item_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &cpp_field_result_item_tmp));
    int32_t cpp_field_result_item = cpp_field_result_item_tmp;
    ::sgl_image::SglImageType cpp_field_result = ::sgl_image::SglImageType::from_value(cpp_field_result_item);
    cpp_ptr->type = cpp_field_result;
    return nullptr;
}
static napi_value sgl_image_SglImageInfo_info_getter0(napi_env env, napi_callback_info info) {
    napi_value thisobj;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisobj, nullptr));
    ::sgl_image::SglImageInfo* cpp_ptr;
    NAPI_CALL(env, napi_unwrap(env, thisobj, reinterpret_cast<void **>(&cpp_ptr)));
    napi_value napi_field_result = sgl_image_Info_intoNAPI0(env, cpp_ptr->info);
    return napi_field_result;
}
static napi_value sgl_image_SglImageInfo_info_setter0(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    napi_value thisobj;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisobj, nullptr));
    ::sgl_image::SglImageInfo* cpp_ptr;
    NAPI_CALL(env, napi_unwrap(env, thisobj, reinterpret_cast<void **>(&cpp_ptr)));
    ::sgl_image::Info cpp_field_result = sgl_image_Info_fromNAPI0(env, args[0]);
    cpp_ptr->info = cpp_field_result;
    return nullptr;
}
inline napi_value sgl_image_SglImageInfo_constructor0_inner(napi_env env, napi_callback_info info) {
    napi_status _status;
    napi_value thisobj;
    size_t argc = 2;
    napi_value args[2];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisobj, nullptr));
    int32_t cpp_field_type_item_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &cpp_field_type_item_tmp));
    int32_t cpp_field_type_item = cpp_field_type_item_tmp;
    ::sgl_image::SglImageType cpp_field_type = ::sgl_image::SglImageType::from_value(cpp_field_type_item);
    ::sgl_image::Info cpp_field_info = sgl_image_Info_fromNAPI0(env, args[1]);
    ::sgl_image::SglImageInfo* cpp_ptr = new ::sgl_image::SglImageInfo{std::move(cpp_field_type), std::move(cpp_field_info)};
    _status = napi_wrap(env, thisobj, cpp_ptr, []([[maybe_unused]] napi_env env, void* finalize_data, [[maybe_unused]] void* finalize_hint) {
        delete static_cast<::sgl_image::SglImageInfo*>(finalize_data);
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
inline napi_ref& ctor_ref_sgl_image_SglImageInfo_t0_inner() {
    static napi_ref instance = nullptr;
    return instance;
}
inline ::sgl_image::SglImageInfo sgl_image_SglImageInfo_fromNAPI0(napi_env env, napi_value napi_obj) {
    napi_value napi_field_type = nullptr;
    NAPI_CALL(env, napi_get_named_property(env, napi_obj, "type", &napi_field_type));
    int32_t cpp_field_type_item_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, napi_field_type, &cpp_field_type_item_tmp));
    int32_t cpp_field_type_item = cpp_field_type_item_tmp;
    ::sgl_image::SglImageType cpp_field_type = ::sgl_image::SglImageType::from_value(cpp_field_type_item);
    napi_value napi_field_info = nullptr;
    NAPI_CALL(env, napi_get_named_property(env, napi_obj, "info", &napi_field_info));
    ::sgl_image::Info cpp_field_info = sgl_image_Info_fromNAPI0(env, napi_field_info);
    return ::sgl_image::SglImageInfo{std::move(cpp_field_type), std::move(cpp_field_info)};
}
inline napi_value sgl_image_SglImageInfo_intoNAPI0(napi_env env, ::sgl_image::SglImageInfo const& cpp_obj) {
    napi_value napi_field_type = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(cpp_obj.type.get_value())), &napi_field_type));
    napi_value napi_field_info = sgl_image_Info_intoNAPI0(env, cpp_obj.info);
    napi_value args[2] = {napi_field_type, napi_field_info};
    napi_value napi_obj = nullptr, constructor = nullptr;
    NAPI_CALL(env, napi_get_reference_value(env, ctor_ref_sgl_image_SglImageInfo_t0_inner(), &constructor));
    NAPI_CALL(env, napi_new_instance(env, constructor, 2, args, &napi_obj));
    return napi_obj;
}
#pragma clang diagnostic pop
