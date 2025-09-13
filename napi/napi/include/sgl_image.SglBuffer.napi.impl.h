#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "sgl_image.SglBuffer.napi.decl.h"
#include "sgl_image.SglBuffer.proj.2.hpp"
#pragma clang diagnostic ignored "-Wmissing-braces"
static napi_value sgl_image_SglBuffer_type_getter0(napi_env env, napi_callback_info info) {
    napi_value thisobj;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisobj, nullptr));
    ::sgl_image::SglBuffer* cpp_ptr;
    NAPI_CALL(env, napi_unwrap(env, thisobj, reinterpret_cast<void **>(&cpp_ptr)));
    napi_value napi_field_result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(cpp_ptr->type.get_value())), &napi_field_result));
    return napi_field_result;
}
static napi_value sgl_image_SglBuffer_type_setter0(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    napi_value thisobj;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisobj, nullptr));
    ::sgl_image::SglBuffer* cpp_ptr;
    NAPI_CALL(env, napi_unwrap(env, thisobj, reinterpret_cast<void **>(&cpp_ptr)));
    int32_t cpp_field_result_item_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &cpp_field_result_item_tmp));
    int32_t cpp_field_result_item = cpp_field_result_item_tmp;
    ::sgl_image::SglBufferType cpp_field_result = ::sgl_image::SglBufferType::from_value(cpp_field_result_item);
    cpp_ptr->type = cpp_field_result;
    return nullptr;
}
static napi_value sgl_image_SglBuffer_bufHandle_ptr_getter03(napi_env env, napi_callback_info info) {
    napi_value thisobj;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisobj, nullptr));
    ::sgl_image::SglBuffer* cpp_ptr;
    NAPI_CALL(env, napi_unwrap(env, thisobj, reinterpret_cast<void **>(&cpp_ptr)));
    napi_value napi_field_result = nullptr;
    NAPI_CALL(env, None(env, cpp_ptr->bufHandle_ptr, &napi_field_result));
    return napi_field_result;
}
static napi_value sgl_image_SglBuffer_bufHandle_ptr_setter03(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    napi_value thisobj;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisobj, nullptr));
    ::sgl_image::SglBuffer* cpp_ptr;
    NAPI_CALL(env, napi_unwrap(env, thisobj, reinterpret_cast<void **>(&cpp_ptr)));
    uint64_t cpp_field_result_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, args[0], &cpp_field_result_tmp));
    uint64_t cpp_field_result = cpp_field_result_tmp;
    cpp_ptr->bufHandle_ptr = cpp_field_result;
    return nullptr;
}
static napi_value sgl_image_SglBuffer_memHandle_ptr_getter03(napi_env env, napi_callback_info info) {
    napi_value thisobj;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisobj, nullptr));
    ::sgl_image::SglBuffer* cpp_ptr;
    NAPI_CALL(env, napi_unwrap(env, thisobj, reinterpret_cast<void **>(&cpp_ptr)));
    napi_value napi_field_result = nullptr;
    NAPI_CALL(env, None(env, cpp_ptr->memHandle_ptr, &napi_field_result));
    return napi_field_result;
}
static napi_value sgl_image_SglBuffer_memHandle_ptr_setter03(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    napi_value thisobj;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisobj, nullptr));
    ::sgl_image::SglBuffer* cpp_ptr;
    NAPI_CALL(env, napi_unwrap(env, thisobj, reinterpret_cast<void **>(&cpp_ptr)));
    uint64_t cpp_field_result_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, args[0], &cpp_field_result_tmp));
    uint64_t cpp_field_result = cpp_field_result_tmp;
    cpp_ptr->memHandle_ptr = cpp_field_result;
    return nullptr;
}
static napi_value sgl_image_SglBuffer_data_ptr_getter03(napi_env env, napi_callback_info info) {
    napi_value thisobj;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisobj, nullptr));
    ::sgl_image::SglBuffer* cpp_ptr;
    NAPI_CALL(env, napi_unwrap(env, thisobj, reinterpret_cast<void **>(&cpp_ptr)));
    napi_value napi_field_result = nullptr;
    NAPI_CALL(env, None(env, cpp_ptr->data_ptr, &napi_field_result));
    return napi_field_result;
}
static napi_value sgl_image_SglBuffer_data_ptr_setter03(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    napi_value thisobj;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisobj, nullptr));
    ::sgl_image::SglBuffer* cpp_ptr;
    NAPI_CALL(env, napi_unwrap(env, thisobj, reinterpret_cast<void **>(&cpp_ptr)));
    uint64_t cpp_field_result_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, args[0], &cpp_field_result_tmp));
    uint64_t cpp_field_result = cpp_field_result_tmp;
    cpp_ptr->data_ptr = cpp_field_result;
    return nullptr;
}
static napi_value sgl_image_SglBuffer_bufferSize_getter0(napi_env env, napi_callback_info info) {
    napi_value thisobj;
    NAPI_CALL(env, napi_get_cb_info(env, info, nullptr, nullptr, &thisobj, nullptr));
    ::sgl_image::SglBuffer* cpp_ptr;
    NAPI_CALL(env, napi_unwrap(env, thisobj, reinterpret_cast<void **>(&cpp_ptr)));
    napi_value napi_field_result = nullptr;
    NAPI_CALL(env, None(env, cpp_ptr->bufferSize, &napi_field_result));
    return napi_field_result;
}
static napi_value sgl_image_SglBuffer_bufferSize_setter0(napi_env env, napi_callback_info info) {
    size_t argc = 1;
    napi_value args[1] = {nullptr};
    napi_value thisobj;
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisobj, nullptr));
    ::sgl_image::SglBuffer* cpp_ptr;
    NAPI_CALL(env, napi_unwrap(env, thisobj, reinterpret_cast<void **>(&cpp_ptr)));
    uint64_t cpp_field_result_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, args[0], &cpp_field_result_tmp));
    uint64_t cpp_field_result = cpp_field_result_tmp;
    cpp_ptr->bufferSize = cpp_field_result;
    return nullptr;
}
inline napi_value sgl_image_SglBuffer_constructor0_inner(napi_env env, napi_callback_info info) {
    napi_status _status;
    napi_value thisobj;
    size_t argc = 5;
    napi_value args[5];
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args, &thisobj, nullptr));
    int32_t cpp_field_type_item_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[0], &cpp_field_type_item_tmp));
    int32_t cpp_field_type_item = cpp_field_type_item_tmp;
    ::sgl_image::SglBufferType cpp_field_type = ::sgl_image::SglBufferType::from_value(cpp_field_type_item);
    uint64_t cpp_field_bufHandle_ptr_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, args[1], &cpp_field_bufHandle_ptr_tmp));
    uint64_t cpp_field_bufHandle_ptr = cpp_field_bufHandle_ptr_tmp;
    uint64_t cpp_field_memHandle_ptr_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, args[2], &cpp_field_memHandle_ptr_tmp));
    uint64_t cpp_field_memHandle_ptr = cpp_field_memHandle_ptr_tmp;
    uint64_t cpp_field_data_ptr_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, args[3], &cpp_field_data_ptr_tmp));
    uint64_t cpp_field_data_ptr = cpp_field_data_ptr_tmp;
    uint64_t cpp_field_bufferSize_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, args[4], &cpp_field_bufferSize_tmp));
    uint64_t cpp_field_bufferSize = cpp_field_bufferSize_tmp;
    ::sgl_image::SglBuffer* cpp_ptr = new ::sgl_image::SglBuffer{std::move(cpp_field_type), std::move(cpp_field_bufHandle_ptr), std::move(cpp_field_memHandle_ptr), std::move(cpp_field_data_ptr), std::move(cpp_field_bufferSize)};
    _status = napi_wrap(env, thisobj, cpp_ptr, []([[maybe_unused]] napi_env env, void* finalize_data, [[maybe_unused]] void* finalize_hint) {
        delete static_cast<::sgl_image::SglBuffer*>(finalize_data);
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
inline napi_ref& ctor_ref_sgl_image_SglBuffer_t0_inner() {
    static napi_ref instance = nullptr;
    return instance;
}
inline ::sgl_image::SglBuffer sgl_image_SglBuffer_fromNAPI0(napi_env env, napi_value napi_obj) {
    napi_value napi_field_type = nullptr;
    NAPI_CALL(env, napi_get_named_property(env, napi_obj, "type", &napi_field_type));
    int32_t cpp_field_type_item_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, napi_field_type, &cpp_field_type_item_tmp));
    int32_t cpp_field_type_item = cpp_field_type_item_tmp;
    ::sgl_image::SglBufferType cpp_field_type = ::sgl_image::SglBufferType::from_value(cpp_field_type_item);
    napi_value napi_field_bufHandle_ptr = nullptr;
    NAPI_CALL(env, napi_get_named_property(env, napi_obj, "bufHandle_ptr", &napi_field_bufHandle_ptr));
    uint64_t cpp_field_bufHandle_ptr_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, napi_field_bufHandle_ptr, &cpp_field_bufHandle_ptr_tmp));
    uint64_t cpp_field_bufHandle_ptr = cpp_field_bufHandle_ptr_tmp;
    napi_value napi_field_memHandle_ptr = nullptr;
    NAPI_CALL(env, napi_get_named_property(env, napi_obj, "memHandle_ptr", &napi_field_memHandle_ptr));
    uint64_t cpp_field_memHandle_ptr_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, napi_field_memHandle_ptr, &cpp_field_memHandle_ptr_tmp));
    uint64_t cpp_field_memHandle_ptr = cpp_field_memHandle_ptr_tmp;
    napi_value napi_field_data_ptr = nullptr;
    NAPI_CALL(env, napi_get_named_property(env, napi_obj, "data_ptr", &napi_field_data_ptr));
    uint64_t cpp_field_data_ptr_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, napi_field_data_ptr, &cpp_field_data_ptr_tmp));
    uint64_t cpp_field_data_ptr = cpp_field_data_ptr_tmp;
    napi_value napi_field_bufferSize = nullptr;
    NAPI_CALL(env, napi_get_named_property(env, napi_obj, "bufferSize", &napi_field_bufferSize));
    uint64_t cpp_field_bufferSize_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, napi_field_bufferSize, &cpp_field_bufferSize_tmp));
    uint64_t cpp_field_bufferSize = cpp_field_bufferSize_tmp;
    return ::sgl_image::SglBuffer{std::move(cpp_field_type), std::move(cpp_field_bufHandle_ptr), std::move(cpp_field_memHandle_ptr), std::move(cpp_field_data_ptr), std::move(cpp_field_bufferSize)};
}
inline napi_value sgl_image_SglBuffer_intoNAPI0(napi_env env, ::sgl_image::SglBuffer const& cpp_obj) {
    napi_value napi_field_type = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(cpp_obj.type.get_value())), &napi_field_type));
    napi_value napi_field_bufHandle_ptr = nullptr;
    NAPI_CALL(env, None(env, cpp_obj.bufHandle_ptr, &napi_field_bufHandle_ptr));
    napi_value napi_field_memHandle_ptr = nullptr;
    NAPI_CALL(env, None(env, cpp_obj.memHandle_ptr, &napi_field_memHandle_ptr));
    napi_value napi_field_data_ptr = nullptr;
    NAPI_CALL(env, None(env, cpp_obj.data_ptr, &napi_field_data_ptr));
    napi_value napi_field_bufferSize = nullptr;
    NAPI_CALL(env, None(env, cpp_obj.bufferSize, &napi_field_bufferSize));
    napi_value args[5] = {napi_field_type, napi_field_bufHandle_ptr, napi_field_memHandle_ptr, napi_field_data_ptr, napi_field_bufferSize};
    napi_value napi_obj = nullptr, constructor = nullptr;
    NAPI_CALL(env, napi_get_reference_value(env, ctor_ref_sgl_image_SglBuffer_t0_inner(), &constructor));
    NAPI_CALL(env, napi_new_instance(env, constructor, 5, args, &napi_obj));
    return napi_obj;
}
#pragma clang diagnostic pop
