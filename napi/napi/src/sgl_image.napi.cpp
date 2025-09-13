#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "sgl_image.napi.h"
#include "sgl_image.user.hpp"
#include "taihe/array.hpp"
#include "sgl_image.SglImageInfo.napi.impl.h"
#include "sgl_image.SglBuffer.napi.impl.h"
#include "sgl_image.SglImageGpuInfo.napi.impl.h"
#include "sgl_image.SglImageCpuInfo.napi.impl.h"
inline bool _taihe_get_msb(uint64_t dig) {
    return dig >> (sizeof(uint64_t) * 8 - 1) != 0;
}
inline bool _taihe_get_sign(taihe::array_view<uint64_t> num) {
    return _taihe_get_msb(num[num.size() - 1]);
}
inline std::pair<bool, taihe::array<uint64_t>> _taihe_get_sign_and_abs(taihe::array_view<uint64_t> num) {
    uint64_t *buf = reinterpret_cast<uint64_t *>(malloc(num.size() * sizeof(uint64_t)));
    bool sign = _taihe_get_msb(num[num.size() - 1]);
    if (sign) {
        bool carry = true;
        for (std::size_t i = 0; i < num.size(); i++) {
            buf[i] = ~num[i] + carry;
            carry = carry && (buf[i] == 0);
        }
    } else {
        for (std::size_t i = 0; i < num.size(); i++) {
            buf[i] = num[i];
        }
    }
    std::size_t size = num.size();
    while (size > 0 && buf[size - 1] == 0) {
        size--;
    }
    return {sign, taihe::array<uint64_t>(buf, size)};
}
inline taihe::array<uint64_t> _taihe_build_num(bool sign, taihe::array_view<uint64_t> abs) {
    uint64_t *buf = reinterpret_cast<uint64_t *>(malloc((abs.size() + 1) * sizeof(uint64_t)));
    if (sign) {
        bool carry = true;
        for (std::size_t i = 0; i < abs.size(); i++) {
            buf[i] = ~abs[i] + carry;
            carry = carry && (buf[i] == 0);
        }
        buf[abs.size()] = carry - 1;
    } else {
        for (std::size_t i = 0; i < abs.size(); i++) {
            buf[i] = abs[i];
        }
        buf[abs.size()] = 0;
    }
    std::size_t size = abs.size() + 1;
    while (size >= 2 && ((buf[size - 1] == 0 && _taihe_get_msb(buf[size - 2]) == 0) ||
                        (buf[size - 1] == static_cast<uint64_t>(-1) && _taihe_get_msb(buf[size - 2]) == 1))) {
        size--;
    }
    return taihe::array<uint64_t>(buf, size);
}
static napi_value sgl_image_gray_NAPI0(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 5;
    napi_value args[5] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    double value2_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[2], &value2_tmp));
    float value2 = value2_tmp;
    double value3_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[3], &value3_tmp));
    float value3 = value3_tmp;
    double value4_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[4], &value4_tmp));
    float value4 = value4_tmp;
    ::sgl_image::SglError value = sgl_image::gray(value0, value1, value2, value3, value4);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_threshold_split_NAPI02(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 3;
    napi_value args[3] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    int32_t value2_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[2], &value2_tmp));
    int32_t value2 = value2_tmp;
    ::sgl_image::SglError value = sgl_image::threshold_split(value0, value1, value2);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_gaussian_blur_NAPI02(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 3;
    napi_value args[3] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    int32_t value2_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[2], &value2_tmp));
    int32_t value2 = value2_tmp;
    ::sgl_image::SglError value = sgl_image::gaussian_blur(value0, value1, value2);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_gaussian_blur_float_NAPI021(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 3;
    napi_value args[3] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    int32_t value2_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[2], &value2_tmp));
    int32_t value2 = value2_tmp;
    ::sgl_image::SglError value = sgl_image::gaussian_blur_float(value0, value1, value2);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_surface_blur_NAPI02(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 4;
    napi_value args[4] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    int32_t value2_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[2], &value2_tmp));
    int32_t value2 = value2_tmp;
    int32_t value3_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[3], &value3_tmp));
    int32_t value3 = value3_tmp;
    ::sgl_image::SglError value = sgl_image::surface_blur(value0, value1, value2, value3);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_distort_glass_NAPI02(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 5;
    napi_value args[5] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    double value2_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[2], &value2_tmp));
    float value2 = value2_tmp;
    double value3_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[3], &value3_tmp));
    float value3 = value3_tmp;
    double value4_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[4], &value4_tmp));
    float value4 = value4_tmp;
    ::sgl_image::SglError value = sgl_image::distort_glass(value0, value1, value2, value3, value4);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_adjust_saturation_NAPI02(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 4;
    napi_value args[4] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    double value2_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[2], &value2_tmp));
    float value2 = value2_tmp;
    double value3_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[3], &value3_tmp));
    float value3 = value3_tmp;
    ::sgl_image::SglError value = sgl_image::adjust_saturation(value0, value1, value2, value3);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_palette_knife_NAPI02(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 4;
    napi_value args[4] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    int32_t value2_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[2], &value2_tmp));
    int32_t value2 = value2_tmp;
    int32_t value3_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[3], &value3_tmp));
    int32_t value3 = value3_tmp;
    ::sgl_image::SglError value = sgl_image::palette_knife(value0, value1, value2, value3);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_hue_equal_NAPI02(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 2;
    napi_value args[2] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    ::sgl_image::SglError value = sgl_image::hue_equal(value0, value1);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_blur_edge_NAPI02(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 5;
    napi_value args[5] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    int32_t value2_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[2], &value2_tmp));
    int32_t value2 = value2_tmp;
    int32_t value3_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[3], &value3_tmp));
    int32_t value3 = value3_tmp;
    int32_t value4_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[4], &value4_tmp));
    int32_t value4 = value4_tmp;
    ::sgl_image::SglError value = sgl_image::blur_edge(value0, value1, value2, value3, value4);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_custom_kernel_NAPI02(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 6;
    napi_value args[6] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    uint64_t value2_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, args[2], &value2_tmp));
    uint64_t value2 = value2_tmp;
    int32_t value3_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[3], &value3_tmp));
    int32_t value3 = value3_tmp;
    int32_t value4_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[4], &value4_tmp));
    int32_t value4 = value4_tmp;
    int32_t value5_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[5], &value5_tmp));
    int32_t value5 = value5_tmp;
    ::sgl_image::SglError value = sgl_image::custom_kernel(value0, value1, value2, value3, value4, value5);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_color_balance_NAPI02(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 5;
    napi_value args[5] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    uint64_t value2_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, args[2], &value2_tmp));
    uint64_t value2 = value2_tmp;
    uint64_t value3_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, args[3], &value3_tmp));
    uint64_t value3 = value3_tmp;
    int32_t value4_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[4], &value4_tmp));
    int32_t value4 = value4_tmp;
    ::sgl_image::SglError value = sgl_image::color_balance(value0, value1, value2, value3, value4);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_black_white_NAPI02(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 4;
    napi_value args[4] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    uint64_t value2_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, args[2], &value2_tmp));
    uint64_t value2 = value2_tmp;
    int32_t value3_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[3], &value3_tmp));
    int32_t value3 = value3_tmp;
    ::sgl_image::SglError value = sgl_image::black_white(value0, value1, value2, value3);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_scale_NAPI0(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 5;
    napi_value args[5] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    int32_t value2_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[2], &value2_tmp));
    int32_t value2 = value2_tmp;
    int32_t value3_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[3], &value3_tmp));
    int32_t value3 = value3_tmp;
    int32_t value4_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[4], &value4_tmp));
    int32_t value4 = value4_tmp;
    ::sgl_image::SglError value = sgl_image::scale(value0, value1, value2, value3, value4);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_color_separation_NAPI02(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 5;
    napi_value args[5] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    double value2_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[2], &value2_tmp));
    float value2 = value2_tmp;
    double value3_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[3], &value3_tmp));
    float value3 = value3_tmp;
    double value4_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[4], &value4_tmp));
    float value4 = value4_tmp;
    ::sgl_image::SglError value = sgl_image::color_separation(value0, value1, value2, value3, value4);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_mid_value_NAPI02(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 4;
    napi_value args[4] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    double value2_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[2], &value2_tmp));
    float value2 = value2_tmp;
    double value3_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[3], &value3_tmp));
    float value3 = value3_tmp;
    ::sgl_image::SglError value = sgl_image::mid_value(value0, value1, value2, value3);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_path_blur_NAPI02(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 11;
    napi_value args[11] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    uint64_t value2_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, args[2], &value2_tmp));
    uint64_t value2 = value2_tmp;
    int32_t value3_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[3], &value3_tmp));
    int32_t value3 = value3_tmp;
    int32_t value4_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[4], &value4_tmp));
    int32_t value4 = value4_tmp;
    int32_t value5_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[5], &value5_tmp));
    int32_t value5 = value5_tmp;
    uint64_t value6_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, args[6], &value6_tmp));
    uint64_t value6 = value6_tmp;
    uint64_t value7_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, args[7], &value7_tmp));
    uint64_t value7 = value7_tmp;
    uint64_t value8_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, args[8], &value8_tmp));
    uint64_t value8 = value8_tmp;
    uint64_t value9_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, args[9], &value9_tmp));
    uint64_t value9 = value9_tmp;
    int32_t value10_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[10], &value10_tmp));
    int32_t value10 = value10_tmp;
    ::sgl_image::SglError value = sgl_image::path_blur(value0, value1, value2, value3, value4, value5, value6, value7, value8, value9, value10);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_crystallize_NAPI0(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 5;
    napi_value args[5] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    uint64_t value2_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, args[2], &value2_tmp));
    uint64_t value2 = value2_tmp;
    uint64_t value3_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, args[3], &value3_tmp));
    uint64_t value3 = value3_tmp;
    int32_t value4_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[4], &value4_tmp));
    int32_t value4 = value4_tmp;
    ::sgl_image::SglError value = sgl_image::crystallize(value0, value1, value2, value3, value4);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_rotation_blur_NAPI02(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 11;
    napi_value args[11] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    ::sgl_image::SglImageInfo value2 = sgl_image_SglImageInfo_fromNAPI0(env, args[2]);
    double value3_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[3], &value3_tmp));
    float value3 = value3_tmp;
    double value4_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[4], &value4_tmp));
    float value4 = value4_tmp;
    double value5_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[5], &value5_tmp));
    float value5 = value5_tmp;
    double value6_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[6], &value6_tmp));
    float value6 = value6_tmp;
    double value7_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[7], &value7_tmp));
    float value7 = value7_tmp;
    double value8_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[8], &value8_tmp));
    float value8 = value8_tmp;
    int32_t value9_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[9], &value9_tmp));
    int32_t value9 = value9_tmp;
    double value10_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[10], &value10_tmp));
    float value10 = value10_tmp;
    ::sgl_image::SglError value = sgl_image::rotation_blur(value0, value1, value2, value3, value4, value5, value6, value7, value8, value9, value10);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_facet_NAPI0(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 4;
    napi_value args[4] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    int32_t value2_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[2], &value2_tmp));
    int32_t value2 = value2_tmp;
    int32_t value3_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[3], &value3_tmp));
    int32_t value3 = value3_tmp;
    ::sgl_image::SglError value = sgl_image::facet(value0, value1, value2, value3);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_accented_edge_NAPI02(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 6;
    napi_value args[6] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    uint64_t value2_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, args[2], &value2_tmp));
    uint64_t value2 = value2_tmp;
    uint64_t value3_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, args[3], &value3_tmp));
    uint64_t value3 = value3_tmp;
    int32_t value4_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[4], &value4_tmp));
    int32_t value4 = value4_tmp;
    int32_t value5_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[5], &value5_tmp));
    int32_t value5 = value5_tmp;
    ::sgl_image::SglError value = sgl_image::accented_edge(value0, value1, value2, value3, value4, value5);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_iris_blur_NAPI02(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 10;
    napi_value args[10] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    ::sgl_image::SglImageInfo value2 = sgl_image_SglImageInfo_fromNAPI0(env, args[2]);
    double value3_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[3], &value3_tmp));
    float value3 = value3_tmp;
    double value4_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[4], &value4_tmp));
    float value4 = value4_tmp;
    double value5_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[5], &value5_tmp));
    float value5 = value5_tmp;
    double value6_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[6], &value6_tmp));
    float value6 = value6_tmp;
    double value7_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[7], &value7_tmp));
    float value7 = value7_tmp;
    double value8_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[8], &value8_tmp));
    float value8 = value8_tmp;
    double value9_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[9], &value9_tmp));
    float value9 = value9_tmp;
    ::sgl_image::SglError value = sgl_image::iris_blur(value0, value1, value2, value3, value4, value5, value6, value7, value8, value9);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_tilt_shift_blur_NAPI021(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 9;
    napi_value args[9] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    ::sgl_image::SglImageInfo value2 = sgl_image_SglImageInfo_fromNAPI0(env, args[2]);
    uint64_t value3_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, args[3], &value3_tmp));
    uint64_t value3 = value3_tmp;
    uint64_t value4_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, args[4], &value4_tmp));
    uint64_t value4 = value4_tmp;
    uint64_t value5_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, args[5], &value5_tmp));
    uint64_t value5 = value5_tmp;
    double value6_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[6], &value6_tmp));
    float value6 = value6_tmp;
    double value7_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[7], &value7_tmp));
    float value7 = value7_tmp;
    int32_t value8_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[8], &value8_tmp));
    int32_t value8 = value8_tmp;
    ::sgl_image::SglError value = sgl_image::tilt_shift_blur(value0, value1, value2, value3, value4, value5, value6, value7, value8);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_radial_blur_NAPI02(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 6;
    napi_value args[6] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    double value2_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[2], &value2_tmp));
    float value2 = value2_tmp;
    int32_t value3_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[3], &value3_tmp));
    int32_t value3 = value3_tmp;
    double value4_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[4], &value4_tmp));
    float value4 = value4_tmp;
    double value5_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[5], &value5_tmp));
    float value5 = value5_tmp;
    ::sgl_image::SglError value = sgl_image::radial_blur(value0, value1, value2, value3, value4, value5);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_rotational_blur_NAPI02(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 6;
    napi_value args[6] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    double value2_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[2], &value2_tmp));
    float value2 = value2_tmp;
    int32_t value3_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[3], &value3_tmp));
    int32_t value3 = value3_tmp;
    double value4_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[4], &value4_tmp));
    float value4 = value4_tmp;
    double value5_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[5], &value5_tmp));
    float value5 = value5_tmp;
    ::sgl_image::SglError value = sgl_image::rotational_blur(value0, value1, value2, value3, value4, value5);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_minmax_NAPI0(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 4;
    napi_value args[4] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    int32_t value2_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[2], &value2_tmp));
    int32_t value2 = value2_tmp;
    int32_t value3_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[3], &value3_tmp));
    int32_t value3 = value3_tmp;
    ::sgl_image::SglError value = sgl_image::minmax(value0, value1, value2, value3);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_color_halftone_NAPI02(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 7;
    napi_value args[7] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    double value2_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[2], &value2_tmp));
    float value2 = value2_tmp;
    double value3_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[3], &value3_tmp));
    float value3 = value3_tmp;
    double value4_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[4], &value4_tmp));
    float value4 = value4_tmp;
    double value5_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[5], &value5_tmp));
    float value5 = value5_tmp;
    uint64_t value6_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, args[6], &value6_tmp));
    uint64_t value6 = value6_tmp;
    ::sgl_image::SglError value = sgl_image::color_halftone(value0, value1, value2, value3, value4, value5, value6);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_sharpen_NAPI0(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 4;
    napi_value args[4] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    uint64_t value2_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, args[2], &value2_tmp));
    uint64_t value2 = value2_tmp;
    int32_t value3_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[3], &value3_tmp));
    int32_t value3 = value3_tmp;
    ::sgl_image::SglError value = sgl_image::sharpen(value0, value1, value2, value3);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_polar_coordinates_NAPI02(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 3;
    napi_value args[3] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    int32_t value2_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[2], &value2_tmp));
    int32_t value2 = value2_tmp;
    ::sgl_image::SglError value = sgl_image::polar_coordinates(value0, value1, value2);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_clouds_NAPI0(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 5;
    napi_value args[5] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    uint64_t value2_tmp;
    NAPI_CALL(env, napi_get_value_bigint_uint64(env, args[2], &value2_tmp));
    uint64_t value2 = value2_tmp;
    int32_t value3_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[3], &value3_tmp));
    int32_t value3 = value3_tmp;
    int32_t value4_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[4], &value4_tmp));
    int32_t value4 = value4_tmp;
    ::sgl_image::SglError value = sgl_image::clouds(value0, value1, value2, value3, value4);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_motion_blur_NAPI02(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 5;
    napi_value args[5] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    int32_t value2_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[2], &value2_tmp));
    int32_t value2 = value2_tmp;
    int32_t value3_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[3], &value3_tmp));
    int32_t value3 = value3_tmp;
    double value4_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[4], &value4_tmp));
    float value4 = value4_tmp;
    ::sgl_image::SglError value = sgl_image::motion_blur(value0, value1, value2, value3, value4);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_twirl_warp_NAPI02(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 3;
    napi_value args[3] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    int32_t value2_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[2], &value2_tmp));
    int32_t value2 = value2_tmp;
    ::sgl_image::SglError value = sgl_image::twirl_warp(value0, value1, value2);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_zigzag_NAPI0(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 6;
    napi_value args[6] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    int32_t value2_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[2], &value2_tmp));
    int32_t value2 = value2_tmp;
    int32_t value3_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[3], &value3_tmp));
    int32_t value3 = value3_tmp;
    int32_t value4_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[4], &value4_tmp));
    int32_t value4 = value4_tmp;
    double value5_tmp;
    NAPI_CALL(env, napi_get_value_double(env, args[5], &value5_tmp));
    float value5 = value5_tmp;
    ::sgl_image::SglError value = sgl_image::zigzag(value0, value1, value2, value3, value4, value5);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_spherize_NAPI0(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 4;
    napi_value args[4] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    int32_t value2_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[2], &value2_tmp));
    int32_t value2 = value2_tmp;
    int32_t value3_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[3], &value3_tmp));
    int32_t value3 = value3_tmp;
    ::sgl_image::SglError value = sgl_image::spherize(value0, value1, value2, value3);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_pinch_NAPI0(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 3;
    napi_value args[3] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    int32_t value2_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[2], &value2_tmp));
    int32_t value2 = value2_tmp;
    ::sgl_image::SglError value = sgl_image::pinch(value0, value1, value2);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
static napi_value sgl_image_wave_NAPI0(napi_env env, [[maybe_unused]] napi_callback_info info) {
    size_t argc = 8;
    napi_value args[8] = {nullptr};
    NAPI_CALL(env, napi_get_cb_info(env, info, &argc, args , nullptr, nullptr));
    ::sgl_image::SglImageInfo value0 = sgl_image_SglImageInfo_fromNAPI0(env, args[0]);
    ::sgl_image::SglImageInfo value1 = sgl_image_SglImageInfo_fromNAPI0(env, args[1]);
    int32_t value2_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[2], &value2_tmp));
    int32_t value2 = value2_tmp;
    int32_t value3_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[3], &value3_tmp));
    int32_t value3 = value3_tmp;
    int32_t value4_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[4], &value4_tmp));
    int32_t value4 = value4_tmp;
    int32_t value5_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[5], &value5_tmp));
    int32_t value5 = value5_tmp;
    int32_t value6_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[6], &value6_tmp));
    int32_t value6 = value6_tmp;
    int32_t value7_tmp;
    NAPI_CALL(env, napi_get_value_int32(env, args[7], &value7_tmp));
    int32_t value7 = value7_tmp;
    ::sgl_image::SglError value = sgl_image::wave(value0, value1, value2, value3, value4, value5, value6, value7);
    napi_value result = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)(value.get_value())), &result));
    return result;
}
inline void sgl_image_SglError_create0(napi_env env, [[maybe_unused]] napi_value exports) {
    napi_value enum_obj;
    napi_create_object(env, &enum_obj);
    napi_value key;
    napi_value value_SGL_SUCCESS = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)0), &value_SGL_SUCCESS));
    NAPI_CALL(env, napi_create_string_utf8(env, "SGL_SUCCESS", NAPI_AUTO_LENGTH, &key));
    NAPI_CALL(env, napi_set_named_property(env, enum_obj, "SGL_SUCCESS", value_SGL_SUCCESS));
    NAPI_CALL(env, napi_set_property(env, enum_obj, value_SGL_SUCCESS, key));
    napi_value value_SGL_FAILURE = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)1), &value_SGL_FAILURE));
    NAPI_CALL(env, napi_create_string_utf8(env, "SGL_FAILURE", NAPI_AUTO_LENGTH, &key));
    NAPI_CALL(env, napi_set_named_property(env, enum_obj, "SGL_FAILURE", value_SGL_FAILURE));
    NAPI_CALL(env, napi_set_property(env, enum_obj, value_SGL_FAILURE, key));
    napi_value value_SGL_GPU_ERROR = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)2), &value_SGL_GPU_ERROR));
    NAPI_CALL(env, napi_create_string_utf8(env, "SGL_GPU_ERROR", NAPI_AUTO_LENGTH, &key));
    NAPI_CALL(env, napi_set_named_property(env, enum_obj, "SGL_GPU_ERROR", value_SGL_GPU_ERROR));
    NAPI_CALL(env, napi_set_property(env, enum_obj, value_SGL_GPU_ERROR, key));
    napi_value value_SGL_OUT_OF_MEMORY = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)3), &value_SGL_OUT_OF_MEMORY));
    NAPI_CALL(env, napi_create_string_utf8(env, "SGL_OUT_OF_MEMORY", NAPI_AUTO_LENGTH, &key));
    NAPI_CALL(env, napi_set_named_property(env, enum_obj, "SGL_OUT_OF_MEMORY", value_SGL_OUT_OF_MEMORY));
    NAPI_CALL(env, napi_set_property(env, enum_obj, value_SGL_OUT_OF_MEMORY, key));
    napi_value value_SGL_INVALID_ARGUMENT = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)4), &value_SGL_INVALID_ARGUMENT));
    NAPI_CALL(env, napi_create_string_utf8(env, "SGL_INVALID_ARGUMENT", NAPI_AUTO_LENGTH, &key));
    NAPI_CALL(env, napi_set_named_property(env, enum_obj, "SGL_INVALID_ARGUMENT", value_SGL_INVALID_ARGUMENT));
    NAPI_CALL(env, napi_set_property(env, enum_obj, value_SGL_INVALID_ARGUMENT, key));
    NAPI_CALL(env, napi_set_named_property(env, exports, "SglError", enum_obj));
    return;
}
inline void sgl_image_SglBufferType_create0(napi_env env, [[maybe_unused]] napi_value exports) {
    napi_value enum_obj;
    napi_create_object(env, &enum_obj);
    napi_value key;
    napi_value value_SGL_BUFFER_TYPE_UNIFORM = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)0), &value_SGL_BUFFER_TYPE_UNIFORM));
    NAPI_CALL(env, napi_create_string_utf8(env, "SGL_BUFFER_TYPE_UNIFORM", NAPI_AUTO_LENGTH, &key));
    NAPI_CALL(env, napi_set_named_property(env, enum_obj, "SGL_BUFFER_TYPE_UNIFORM", value_SGL_BUFFER_TYPE_UNIFORM));
    NAPI_CALL(env, napi_set_property(env, enum_obj, value_SGL_BUFFER_TYPE_UNIFORM, key));
    napi_value value_SGL_BUFFER_TYPE_STORAGE_LOCAL = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)1), &value_SGL_BUFFER_TYPE_STORAGE_LOCAL));
    NAPI_CALL(env, napi_create_string_utf8(env, "SGL_BUFFER_TYPE_STORAGE_LOCAL", NAPI_AUTO_LENGTH, &key));
    NAPI_CALL(env, napi_set_named_property(env, enum_obj, "SGL_BUFFER_TYPE_STORAGE_LOCAL", value_SGL_BUFFER_TYPE_STORAGE_LOCAL));
    NAPI_CALL(env, napi_set_property(env, enum_obj, value_SGL_BUFFER_TYPE_STORAGE_LOCAL, key));
    napi_value value_SGL_BUFFER_TYPE_STORAGE_SHARED = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)2), &value_SGL_BUFFER_TYPE_STORAGE_SHARED));
    NAPI_CALL(env, napi_create_string_utf8(env, "SGL_BUFFER_TYPE_STORAGE_SHARED", NAPI_AUTO_LENGTH, &key));
    NAPI_CALL(env, napi_set_named_property(env, enum_obj, "SGL_BUFFER_TYPE_STORAGE_SHARED", value_SGL_BUFFER_TYPE_STORAGE_SHARED));
    NAPI_CALL(env, napi_set_property(env, enum_obj, value_SGL_BUFFER_TYPE_STORAGE_SHARED, key));
    napi_value value_SGL_BUFFER_TYPE_VERTEX = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)3), &value_SGL_BUFFER_TYPE_VERTEX));
    NAPI_CALL(env, napi_create_string_utf8(env, "SGL_BUFFER_TYPE_VERTEX", NAPI_AUTO_LENGTH, &key));
    NAPI_CALL(env, napi_set_named_property(env, enum_obj, "SGL_BUFFER_TYPE_VERTEX", value_SGL_BUFFER_TYPE_VERTEX));
    NAPI_CALL(env, napi_set_property(env, enum_obj, value_SGL_BUFFER_TYPE_VERTEX, key));
    napi_value value_SGL_BUFFER_TYPE_INDEX = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)4), &value_SGL_BUFFER_TYPE_INDEX));
    NAPI_CALL(env, napi_create_string_utf8(env, "SGL_BUFFER_TYPE_INDEX", NAPI_AUTO_LENGTH, &key));
    NAPI_CALL(env, napi_set_named_property(env, enum_obj, "SGL_BUFFER_TYPE_INDEX", value_SGL_BUFFER_TYPE_INDEX));
    NAPI_CALL(env, napi_set_property(env, enum_obj, value_SGL_BUFFER_TYPE_INDEX, key));
    napi_value value_SGL_BUFFER_TYPE_UNKNOWN = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)5), &value_SGL_BUFFER_TYPE_UNKNOWN));
    NAPI_CALL(env, napi_create_string_utf8(env, "SGL_BUFFER_TYPE_UNKNOWN", NAPI_AUTO_LENGTH, &key));
    NAPI_CALL(env, napi_set_named_property(env, enum_obj, "SGL_BUFFER_TYPE_UNKNOWN", value_SGL_BUFFER_TYPE_UNKNOWN));
    NAPI_CALL(env, napi_set_property(env, enum_obj, value_SGL_BUFFER_TYPE_UNKNOWN, key));
    NAPI_CALL(env, napi_set_named_property(env, exports, "SglBufferType", enum_obj));
    return;
}
inline void sgl_image_SglImageType_create0(napi_env env, [[maybe_unused]] napi_value exports) {
    napi_value enum_obj;
    napi_create_object(env, &enum_obj);
    napi_value key;
    napi_value value_SGL_IMAGE_TYPE_CPU = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)0), &value_SGL_IMAGE_TYPE_CPU));
    NAPI_CALL(env, napi_create_string_utf8(env, "SGL_IMAGE_TYPE_CPU", NAPI_AUTO_LENGTH, &key));
    NAPI_CALL(env, napi_set_named_property(env, enum_obj, "SGL_IMAGE_TYPE_CPU", value_SGL_IMAGE_TYPE_CPU));
    NAPI_CALL(env, napi_set_property(env, enum_obj, value_SGL_IMAGE_TYPE_CPU, key));
    napi_value value_SGL_IMAGE_TYPE_GPU = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)1), &value_SGL_IMAGE_TYPE_GPU));
    NAPI_CALL(env, napi_create_string_utf8(env, "SGL_IMAGE_TYPE_GPU", NAPI_AUTO_LENGTH, &key));
    NAPI_CALL(env, napi_set_named_property(env, enum_obj, "SGL_IMAGE_TYPE_GPU", value_SGL_IMAGE_TYPE_GPU));
    NAPI_CALL(env, napi_set_property(env, enum_obj, value_SGL_IMAGE_TYPE_GPU, key));
    napi_value value_SGL_IMAGE_TYPE_UNKNOWN = nullptr;
    NAPI_CALL(env, napi_create_int32(env, ((int32_t)2), &value_SGL_IMAGE_TYPE_UNKNOWN));
    NAPI_CALL(env, napi_create_string_utf8(env, "SGL_IMAGE_TYPE_UNKNOWN", NAPI_AUTO_LENGTH, &key));
    NAPI_CALL(env, napi_set_named_property(env, enum_obj, "SGL_IMAGE_TYPE_UNKNOWN", value_SGL_IMAGE_TYPE_UNKNOWN));
    NAPI_CALL(env, napi_set_property(env, enum_obj, value_SGL_IMAGE_TYPE_UNKNOWN, key));
    NAPI_CALL(env, napi_set_named_property(env, exports, "SglImageType", enum_obj));
    return;
}
inline void sgl_image_SglBuffer_create0(napi_env env, [[maybe_unused]] napi_value exports) {
    napi_value result = nullptr;
    napi_property_descriptor desc[] = {
        {"type", nullptr, nullptr, sgl_image_SglBuffer_type_getter0, sgl_image_SglBuffer_type_setter0, nullptr, napi_default, nullptr}, 
        {"bufHandle_ptr", nullptr, nullptr, sgl_image_SglBuffer_bufHandle_ptr_getter03, sgl_image_SglBuffer_bufHandle_ptr_setter03, nullptr, napi_default, nullptr}, 
        {"memHandle_ptr", nullptr, nullptr, sgl_image_SglBuffer_memHandle_ptr_getter03, sgl_image_SglBuffer_memHandle_ptr_setter03, nullptr, napi_default, nullptr}, 
        {"data_ptr", nullptr, nullptr, sgl_image_SglBuffer_data_ptr_getter03, sgl_image_SglBuffer_data_ptr_setter03, nullptr, napi_default, nullptr}, 
        {"bufferSize", nullptr, nullptr, sgl_image_SglBuffer_bufferSize_getter0, sgl_image_SglBuffer_bufferSize_setter0, nullptr, napi_default, nullptr}, 
    };
    NAPI_CALL(env, napi_define_class(env, "SglBuffer_inner", NAPI_AUTO_LENGTH, sgl_image_SglBuffer_constructor0_inner, nullptr, sizeof(desc) / sizeof(desc[0]), desc, &result));
    NAPI_CALL(env, napi_create_reference(env, result, 1, &ctor_ref_sgl_image_SglBuffer_t0_inner()));
    return;
}
inline void sgl_image_SglImageGpuInfo_create0(napi_env env, [[maybe_unused]] napi_value exports) {
    napi_value result = nullptr;
    napi_property_descriptor desc[] = {
        {"width", nullptr, nullptr, sgl_image_SglImageGpuInfo_width_getter0, sgl_image_SglImageGpuInfo_width_setter0, nullptr, napi_default, nullptr}, 
        {"height", nullptr, nullptr, sgl_image_SglImageGpuInfo_height_getter0, sgl_image_SglImageGpuInfo_height_setter0, nullptr, napi_default, nullptr}, 
        {"channels", nullptr, nullptr, sgl_image_SglImageGpuInfo_channels_getter0, sgl_image_SglImageGpuInfo_channels_setter0, nullptr, napi_default, nullptr}, 
        {"bytesPerLine", nullptr, nullptr, sgl_image_SglImageGpuInfo_bytesPerLine_getter0, sgl_image_SglImageGpuInfo_bytesPerLine_setter0, nullptr, napi_default, nullptr}, 
        {"gpuBuf", nullptr, nullptr, sgl_image_SglImageGpuInfo_gpuBuf_getter0, sgl_image_SglImageGpuInfo_gpuBuf_setter0, nullptr, napi_default, nullptr}, 
    };
    NAPI_CALL(env, napi_define_class(env, "SglImageGpuInfo_inner", NAPI_AUTO_LENGTH, sgl_image_SglImageGpuInfo_constructor0_inner, nullptr, sizeof(desc) / sizeof(desc[0]), desc, &result));
    NAPI_CALL(env, napi_create_reference(env, result, 1, &ctor_ref_sgl_image_SglImageGpuInfo_t0_inner()));
    return;
}
inline void sgl_image_SglImageCpuInfo_create0(napi_env env, [[maybe_unused]] napi_value exports) {
    napi_value result = nullptr;
    napi_property_descriptor desc[] = {
        {"width", nullptr, nullptr, sgl_image_SglImageCpuInfo_width_getter0, sgl_image_SglImageCpuInfo_width_setter0, nullptr, napi_default, nullptr}, 
        {"height", nullptr, nullptr, sgl_image_SglImageCpuInfo_height_getter0, sgl_image_SglImageCpuInfo_height_setter0, nullptr, napi_default, nullptr}, 
        {"channels", nullptr, nullptr, sgl_image_SglImageCpuInfo_channels_getter0, sgl_image_SglImageCpuInfo_channels_setter0, nullptr, napi_default, nullptr}, 
        {"bytesPerLine", nullptr, nullptr, sgl_image_SglImageCpuInfo_bytesPerLine_getter0, sgl_image_SglImageCpuInfo_bytesPerLine_setter0, nullptr, napi_default, nullptr}, 
        {"data_ptr", nullptr, nullptr, sgl_image_SglImageCpuInfo_data_ptr_getter03, sgl_image_SglImageCpuInfo_data_ptr_setter03, nullptr, napi_default, nullptr}, 
    };
    NAPI_CALL(env, napi_define_class(env, "SglImageCpuInfo_inner", NAPI_AUTO_LENGTH, sgl_image_SglImageCpuInfo_constructor0_inner, nullptr, sizeof(desc) / sizeof(desc[0]), desc, &result));
    NAPI_CALL(env, napi_create_reference(env, result, 1, &ctor_ref_sgl_image_SglImageCpuInfo_t0_inner()));
    return;
}
inline void sgl_image_SglImageInfo_create0(napi_env env, [[maybe_unused]] napi_value exports) {
    napi_value result = nullptr;
    napi_property_descriptor desc[] = {
        {"type", nullptr, nullptr, sgl_image_SglImageInfo_type_getter0, sgl_image_SglImageInfo_type_setter0, nullptr, napi_default, nullptr}, 
        {"info", nullptr, nullptr, sgl_image_SglImageInfo_info_getter0, sgl_image_SglImageInfo_info_setter0, nullptr, napi_default, nullptr}, 
    };
    NAPI_CALL(env, napi_define_class(env, "SglImageInfo_inner", NAPI_AUTO_LENGTH, sgl_image_SglImageInfo_constructor0_inner, nullptr, sizeof(desc) / sizeof(desc[0]), desc, &result));
    NAPI_CALL(env, napi_create_reference(env, result, 1, &ctor_ref_sgl_image_SglImageInfo_t0_inner()));
    return;
}
EXTERN_C_START
napi_value Init__sgl_image(napi_env env, napi_value exports) {
    if (::taihe::get_env() == nullptr) {
        ::taihe::set_env(env);
    }
    sgl_image_SglBuffer_create0(env, exports);
    sgl_image_SglImageGpuInfo_create0(env, exports);
    sgl_image_SglImageCpuInfo_create0(env, exports);
    sgl_image_SglImageInfo_create0(env, exports);
    sgl_image_SglError_create0(env, exports);
    sgl_image_SglBufferType_create0(env, exports);
    sgl_image_SglImageType_create0(env, exports);
    napi_property_descriptor desc[] = {
        {"gray", nullptr, sgl_image_gray_NAPI0, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"threshold_split", nullptr, sgl_image_threshold_split_NAPI02, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"gaussian_blur", nullptr, sgl_image_gaussian_blur_NAPI02, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"gaussian_blur_float", nullptr, sgl_image_gaussian_blur_float_NAPI021, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"surface_blur", nullptr, sgl_image_surface_blur_NAPI02, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"distort_glass", nullptr, sgl_image_distort_glass_NAPI02, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"adjust_saturation", nullptr, sgl_image_adjust_saturation_NAPI02, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"palette_knife", nullptr, sgl_image_palette_knife_NAPI02, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"hue_equal", nullptr, sgl_image_hue_equal_NAPI02, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"blur_edge", nullptr, sgl_image_blur_edge_NAPI02, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"custom_kernel", nullptr, sgl_image_custom_kernel_NAPI02, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"color_balance", nullptr, sgl_image_color_balance_NAPI02, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"black_white", nullptr, sgl_image_black_white_NAPI02, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"scale", nullptr, sgl_image_scale_NAPI0, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"color_separation", nullptr, sgl_image_color_separation_NAPI02, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"mid_value", nullptr, sgl_image_mid_value_NAPI02, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"path_blur", nullptr, sgl_image_path_blur_NAPI02, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"crystallize", nullptr, sgl_image_crystallize_NAPI0, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"rotation_blur", nullptr, sgl_image_rotation_blur_NAPI02, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"facet", nullptr, sgl_image_facet_NAPI0, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"accented_edge", nullptr, sgl_image_accented_edge_NAPI02, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"iris_blur", nullptr, sgl_image_iris_blur_NAPI02, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"tilt_shift_blur", nullptr, sgl_image_tilt_shift_blur_NAPI021, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"radial_blur", nullptr, sgl_image_radial_blur_NAPI02, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"rotational_blur", nullptr, sgl_image_rotational_blur_NAPI02, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"minmax", nullptr, sgl_image_minmax_NAPI0, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"color_halftone", nullptr, sgl_image_color_halftone_NAPI02, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"sharpen", nullptr, sgl_image_sharpen_NAPI0, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"polar_coordinates", nullptr, sgl_image_polar_coordinates_NAPI02, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"clouds", nullptr, sgl_image_clouds_NAPI0, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"motion_blur", nullptr, sgl_image_motion_blur_NAPI02, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"twirl_warp", nullptr, sgl_image_twirl_warp_NAPI02, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"zigzag", nullptr, sgl_image_zigzag_NAPI0, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"spherize", nullptr, sgl_image_spherize_NAPI0, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"pinch", nullptr, sgl_image_pinch_NAPI0, nullptr, nullptr, nullptr, napi_default, nullptr}, 
        {"wave", nullptr, sgl_image_wave_NAPI0, nullptr, nullptr, nullptr, napi_default, nullptr}, 
    };
    napi_define_properties(env, exports, sizeof(desc) / sizeof(desc[0]), desc);
    return exports;
}
EXTERN_C_END
#pragma clang diagnostic pop
