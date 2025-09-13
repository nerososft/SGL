#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "taihe/common.hpp"
#include "sgl_image.abi.h"
#include "sgl_image.SglImageInfo.proj.2.hpp"
#include "sgl_image.SglError.proj.1.hpp"
#define TH_EXPORT_CPP_API_gray(CPP_FUNC_IMPL) \
    int sgl_image_gray_f0(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, float r, float g, float b) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<float>(r), ::taihe::from_abi<float>(g), ::taihe::from_abi<float>(b))); \
    }
#define TH_EXPORT_CPP_API_threshold_split(CPP_FUNC_IMPL) \
    int sgl_image_threshold_split_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t bright) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<int32_t>(bright))); \
    }
#define TH_EXPORT_CPP_API_gaussian_blur(CPP_FUNC_IMPL) \
    int sgl_image_gaussian_blur_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t r) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<int32_t>(r))); \
    }
#define TH_EXPORT_CPP_API_gaussian_blur_float(CPP_FUNC_IMPL) \
    int sgl_image_gaussian_blur_float_f021(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t r) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<int32_t>(r))); \
    }
#define TH_EXPORT_CPP_API_surface_blur(CPP_FUNC_IMPL) \
    int sgl_image_surface_blur_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t r, int32_t th) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<int32_t>(r), ::taihe::from_abi<int32_t>(th))); \
    }
#define TH_EXPORT_CPP_API_distort_glass(CPP_FUNC_IMPL) \
    int sgl_image_distort_glass_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, float scale, float intensity, float zoom) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<float>(scale), ::taihe::from_abi<float>(intensity), ::taihe::from_abi<float>(zoom))); \
    }
#define TH_EXPORT_CPP_API_adjust_saturation(CPP_FUNC_IMPL) \
    int sgl_image_adjust_saturation_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, float v, float s) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<float>(v), ::taihe::from_abi<float>(s))); \
    }
#define TH_EXPORT_CPP_API_palette_knife(CPP_FUNC_IMPL) \
    int sgl_image_palette_knife_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t r, int32_t s) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<int32_t>(r), ::taihe::from_abi<int32_t>(s))); \
    }
#define TH_EXPORT_CPP_API_hue_equal(CPP_FUNC_IMPL) \
    int sgl_image_hue_equal_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out))); \
    }
#define TH_EXPORT_CPP_API_blur_edge(CPP_FUNC_IMPL) \
    int sgl_image_blur_edge_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t r, int32_t s, int32_t kernel_type) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<int32_t>(r), ::taihe::from_abi<int32_t>(s), ::taihe::from_abi<int32_t>(kernel_type))); \
    }
#define TH_EXPORT_CPP_API_custom_kernel(CPP_FUNC_IMPL) \
    int sgl_image_custom_kernel_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, uint64_t k_i32ptr, int32_t radius, int32_t offset, int32_t scale) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<uint64_t>(k_i32ptr), ::taihe::from_abi<int32_t>(radius), ::taihe::from_abi<int32_t>(offset), ::taihe::from_abi<int32_t>(scale))); \
    }
#define TH_EXPORT_CPP_API_color_balance(CPP_FUNC_IMPL) \
    int sgl_image_color_balance_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, uint64_t adjustP_fptr, uint64_t p_i32ptr, int32_t l) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<uint64_t>(adjustP_fptr), ::taihe::from_abi<uint64_t>(p_i32ptr), ::taihe::from_abi<int32_t>(l))); \
    }
#define TH_EXPORT_CPP_API_black_white(CPP_FUNC_IMPL) \
    int sgl_image_black_white_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, uint64_t weight_fptr, int32_t wSize) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<uint64_t>(weight_fptr), ::taihe::from_abi<int32_t>(wSize))); \
    }
#define TH_EXPORT_CPP_API_scale(CPP_FUNC_IMPL) \
    int sgl_image_scale_f0(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t weight, int32_t height, int32_t type) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<int32_t>(weight), ::taihe::from_abi<int32_t>(height), ::taihe::from_abi<int32_t>(type))); \
    }
#define TH_EXPORT_CPP_API_color_separation(CPP_FUNC_IMPL) \
    int sgl_image_color_separation_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, float roff, float goff, float boff) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<float>(roff), ::taihe::from_abi<float>(goff), ::taihe::from_abi<float>(boff))); \
    }
#define TH_EXPORT_CPP_API_mid_value(CPP_FUNC_IMPL) \
    int sgl_image_mid_value_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, float radius, float threshold) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<float>(radius), ::taihe::from_abi<float>(threshold))); \
    }
#define TH_EXPORT_CPP_API_path_blur(CPP_FUNC_IMPL) \
    int sgl_image_path_blur_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, uint64_t vec_fptr, int32_t amount, int32_t width, int32_t height, uint64_t startpos_fptr, uint64_t endpos_fptr, uint64_t startvec_fptr, uint64_t endvec_fptr, int32_t num) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<uint64_t>(vec_fptr), ::taihe::from_abi<int32_t>(amount), ::taihe::from_abi<int32_t>(width), ::taihe::from_abi<int32_t>(height), ::taihe::from_abi<uint64_t>(startpos_fptr), ::taihe::from_abi<uint64_t>(endpos_fptr), ::taihe::from_abi<uint64_t>(startvec_fptr), ::taihe::from_abi<uint64_t>(endvec_fptr), ::taihe::from_abi<int32_t>(num))); \
    }
#define TH_EXPORT_CPP_API_crystallize(CPP_FUNC_IMPL) \
    int sgl_image_crystallize_f0(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, uint64_t posx_fptr, uint64_t posy_fptr, int32_t n) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<uint64_t>(posx_fptr), ::taihe::from_abi<uint64_t>(posy_fptr), ::taihe::from_abi<int32_t>(n))); \
    }
#define TH_EXPORT_CPP_API_rotation_blur(CPP_FUNC_IMPL) \
    int sgl_image_rotation_blur_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* in2, struct sgl_image_SglImageInfo_t0 const* out, float x, float y, float a, float b, float ina, float inb, int32_t strength, float angle) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(in2), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<float>(x), ::taihe::from_abi<float>(y), ::taihe::from_abi<float>(a), ::taihe::from_abi<float>(b), ::taihe::from_abi<float>(ina), ::taihe::from_abi<float>(inb), ::taihe::from_abi<int32_t>(strength), ::taihe::from_abi<float>(angle))); \
    }
#define TH_EXPORT_CPP_API_facet(CPP_FUNC_IMPL) \
    int sgl_image_facet_f0(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t radius, int32_t intensitylevel) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<int32_t>(radius), ::taihe::from_abi<int32_t>(intensitylevel))); \
    }
#define TH_EXPORT_CPP_API_accented_edge(CPP_FUNC_IMPL) \
    int sgl_image_accented_edge_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, uint64_t sobelx_i32ptr, uint64_t sobely_i32ptr, int32_t size, int32_t type) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<uint64_t>(sobelx_i32ptr), ::taihe::from_abi<uint64_t>(sobely_i32ptr), ::taihe::from_abi<int32_t>(size), ::taihe::from_abi<int32_t>(type))); \
    }
#define TH_EXPORT_CPP_API_iris_blur(CPP_FUNC_IMPL) \
    int sgl_image_iris_blur_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* in2, struct sgl_image_SglImageInfo_t0 const* out, float x, float y, float a, float b, float ina, float inb, float angle) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(in2), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<float>(x), ::taihe::from_abi<float>(y), ::taihe::from_abi<float>(a), ::taihe::from_abi<float>(b), ::taihe::from_abi<float>(ina), ::taihe::from_abi<float>(inb), ::taihe::from_abi<float>(angle))); \
    }
#define TH_EXPORT_CPP_API_tilt_shift_blur(CPP_FUNC_IMPL) \
    int sgl_image_tilt_shift_blur_f021(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* in2, struct sgl_image_SglImageInfo_t0 const* out, uint64_t A_fptr, uint64_t B_fptr, uint64_t C_fptr, float xoffset, float yoffset, int32_t size) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(in2), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<uint64_t>(A_fptr), ::taihe::from_abi<uint64_t>(B_fptr), ::taihe::from_abi<uint64_t>(C_fptr), ::taihe::from_abi<float>(xoffset), ::taihe::from_abi<float>(yoffset), ::taihe::from_abi<int32_t>(size))); \
    }
#define TH_EXPORT_CPP_API_radial_blur(CPP_FUNC_IMPL) \
    int sgl_image_radial_blur_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, float sharpness, int32_t strength, float xCenter, float yCenter) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<float>(sharpness), ::taihe::from_abi<int32_t>(strength), ::taihe::from_abi<float>(xCenter), ::taihe::from_abi<float>(yCenter))); \
    }
#define TH_EXPORT_CPP_API_rotational_blur(CPP_FUNC_IMPL) \
    int sgl_image_rotational_blur_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, float angle, int32_t strength, float x, float y) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<float>(angle), ::taihe::from_abi<int32_t>(strength), ::taihe::from_abi<float>(x), ::taihe::from_abi<float>(y))); \
    }
#define TH_EXPORT_CPP_API_minmax(CPP_FUNC_IMPL) \
    int sgl_image_minmax_f0(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t radius, int32_t type) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<int32_t>(radius), ::taihe::from_abi<int32_t>(type))); \
    }
#define TH_EXPORT_CPP_API_color_halftone(CPP_FUNC_IMPL) \
    int sgl_image_color_halftone_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, float cyanAngle, float yellowAngle, float magentaAngle, float radius, uint64_t lookup_fptr) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<float>(cyanAngle), ::taihe::from_abi<float>(yellowAngle), ::taihe::from_abi<float>(magentaAngle), ::taihe::from_abi<float>(radius), ::taihe::from_abi<uint64_t>(lookup_fptr))); \
    }
#define TH_EXPORT_CPP_API_sharpen(CPP_FUNC_IMPL) \
    int sgl_image_sharpen_f0(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, uint64_t kernel_i32ptr, int32_t size) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<uint64_t>(kernel_i32ptr), ::taihe::from_abi<int32_t>(size))); \
    }
#define TH_EXPORT_CPP_API_polar_coordinates(CPP_FUNC_IMPL) \
    int sgl_image_polar_coordinates_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t type) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<int32_t>(type))); \
    }
#define TH_EXPORT_CPP_API_clouds(CPP_FUNC_IMPL) \
    int sgl_image_clouds_f0(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, uint64_t permuteLookup_i32ptr, int32_t size, int32_t type) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<uint64_t>(permuteLookup_i32ptr), ::taihe::from_abi<int32_t>(size), ::taihe::from_abi<int32_t>(type))); \
    }
#define TH_EXPORT_CPP_API_motion_blur(CPP_FUNC_IMPL) \
    int sgl_image_motion_blur_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t distance, int32_t angle, float proportion) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<int32_t>(distance), ::taihe::from_abi<int32_t>(angle), ::taihe::from_abi<float>(proportion))); \
    }
#define TH_EXPORT_CPP_API_twirl_warp(CPP_FUNC_IMPL) \
    int sgl_image_twirl_warp_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t angle) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<int32_t>(angle))); \
    }
#define TH_EXPORT_CPP_API_zigzag(CPP_FUNC_IMPL) \
    int sgl_image_zigzag_f0(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t wavelength, int32_t amplitude, int32_t type_wave, float proportion) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<int32_t>(wavelength), ::taihe::from_abi<int32_t>(amplitude), ::taihe::from_abi<int32_t>(type_wave), ::taihe::from_abi<float>(proportion))); \
    }
#define TH_EXPORT_CPP_API_spherize(CPP_FUNC_IMPL) \
    int sgl_image_spherize_f0(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t alpha, int32_t type) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<int32_t>(alpha), ::taihe::from_abi<int32_t>(type))); \
    }
#define TH_EXPORT_CPP_API_pinch(CPP_FUNC_IMPL) \
    int sgl_image_pinch_f0(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t amount) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<int32_t>(amount))); \
    }
#define TH_EXPORT_CPP_API_wave(CPP_FUNC_IMPL) \
    int sgl_image_wave_f0(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t wavelength, int32_t amplitude, int32_t x_pro, int32_t y_pro, int32_t type, int32_t method) { \
        return ::taihe::into_abi<::sgl_image::SglError>(CPP_FUNC_IMPL(::taihe::from_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::from_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::from_abi<int32_t>(wavelength), ::taihe::from_abi<int32_t>(amplitude), ::taihe::from_abi<int32_t>(x_pro), ::taihe::from_abi<int32_t>(y_pro), ::taihe::from_abi<int32_t>(type), ::taihe::from_abi<int32_t>(method))); \
    }
#pragma clang diagnostic pop
