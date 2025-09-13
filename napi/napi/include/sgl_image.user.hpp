#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "sgl_image.proj.hpp"
#include "taihe/common.hpp"
#include "sgl_image.abi.h"
#include "sgl_image.SglImageInfo.proj.2.hpp"
#include "sgl_image.SglError.proj.1.hpp"
namespace sgl_image {
inline ::sgl_image::SglError gray(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, float r, float g, float b) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_gray_f0(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<float>(r), ::taihe::into_abi<float>(g), ::taihe::into_abi<float>(b)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError threshold_split(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t bright) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_threshold_split_f02(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<int32_t>(bright)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError gaussian_blur(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t r) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_gaussian_blur_f02(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<int32_t>(r)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError gaussian_blur_float(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t r) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_gaussian_blur_float_f021(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<int32_t>(r)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError surface_blur(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t r, int32_t th) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_surface_blur_f02(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<int32_t>(r), ::taihe::into_abi<int32_t>(th)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError distort_glass(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, float scale, float intensity, float zoom) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_distort_glass_f02(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<float>(scale), ::taihe::into_abi<float>(intensity), ::taihe::into_abi<float>(zoom)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError adjust_saturation(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, float v, float s) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_adjust_saturation_f02(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<float>(v), ::taihe::into_abi<float>(s)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError palette_knife(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t r, int32_t s) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_palette_knife_f02(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<int32_t>(r), ::taihe::into_abi<int32_t>(s)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError hue_equal(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_hue_equal_f02(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError blur_edge(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t r, int32_t s, int32_t kernel_type) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_blur_edge_f02(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<int32_t>(r), ::taihe::into_abi<int32_t>(s), ::taihe::into_abi<int32_t>(kernel_type)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError custom_kernel(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, uint64_t k_i32ptr, int32_t radius, int32_t offset, int32_t scale) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_custom_kernel_f02(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<uint64_t>(k_i32ptr), ::taihe::into_abi<int32_t>(radius), ::taihe::into_abi<int32_t>(offset), ::taihe::into_abi<int32_t>(scale)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError color_balance(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, uint64_t adjustP_fptr, uint64_t p_i32ptr, int32_t l) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_color_balance_f02(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<uint64_t>(adjustP_fptr), ::taihe::into_abi<uint64_t>(p_i32ptr), ::taihe::into_abi<int32_t>(l)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError black_white(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, uint64_t weight_fptr, int32_t wSize) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_black_white_f02(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<uint64_t>(weight_fptr), ::taihe::into_abi<int32_t>(wSize)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError scale(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t weight, int32_t height, int32_t type) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_scale_f0(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<int32_t>(weight), ::taihe::into_abi<int32_t>(height), ::taihe::into_abi<int32_t>(type)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError color_separation(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, float roff, float goff, float boff) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_color_separation_f02(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<float>(roff), ::taihe::into_abi<float>(goff), ::taihe::into_abi<float>(boff)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError mid_value(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, float radius, float threshold) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_mid_value_f02(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<float>(radius), ::taihe::into_abi<float>(threshold)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError path_blur(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, uint64_t vec_fptr, int32_t amount, int32_t width, int32_t height, uint64_t startpos_fptr, uint64_t endpos_fptr, uint64_t startvec_fptr, uint64_t endvec_fptr, int32_t num) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_path_blur_f02(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<uint64_t>(vec_fptr), ::taihe::into_abi<int32_t>(amount), ::taihe::into_abi<int32_t>(width), ::taihe::into_abi<int32_t>(height), ::taihe::into_abi<uint64_t>(startpos_fptr), ::taihe::into_abi<uint64_t>(endpos_fptr), ::taihe::into_abi<uint64_t>(startvec_fptr), ::taihe::into_abi<uint64_t>(endvec_fptr), ::taihe::into_abi<int32_t>(num)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError crystallize(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, uint64_t posx_fptr, uint64_t posy_fptr, int32_t n) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_crystallize_f0(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<uint64_t>(posx_fptr), ::taihe::into_abi<uint64_t>(posy_fptr), ::taihe::into_abi<int32_t>(n)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError rotation_blur(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& in2, ::sgl_image::SglImageInfo const& out, float x, float y, float a, float b, float ina, float inb, int32_t strength, float angle) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_rotation_blur_f02(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(in2), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<float>(x), ::taihe::into_abi<float>(y), ::taihe::into_abi<float>(a), ::taihe::into_abi<float>(b), ::taihe::into_abi<float>(ina), ::taihe::into_abi<float>(inb), ::taihe::into_abi<int32_t>(strength), ::taihe::into_abi<float>(angle)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError facet(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t radius, int32_t intensitylevel) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_facet_f0(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<int32_t>(radius), ::taihe::into_abi<int32_t>(intensitylevel)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError accented_edge(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, uint64_t sobelx_i32ptr, uint64_t sobely_i32ptr, int32_t size, int32_t type) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_accented_edge_f02(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<uint64_t>(sobelx_i32ptr), ::taihe::into_abi<uint64_t>(sobely_i32ptr), ::taihe::into_abi<int32_t>(size), ::taihe::into_abi<int32_t>(type)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError iris_blur(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& in2, ::sgl_image::SglImageInfo const& out, float x, float y, float a, float b, float ina, float inb, float angle) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_iris_blur_f02(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(in2), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<float>(x), ::taihe::into_abi<float>(y), ::taihe::into_abi<float>(a), ::taihe::into_abi<float>(b), ::taihe::into_abi<float>(ina), ::taihe::into_abi<float>(inb), ::taihe::into_abi<float>(angle)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError tilt_shift_blur(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& in2, ::sgl_image::SglImageInfo const& out, uint64_t A_fptr, uint64_t B_fptr, uint64_t C_fptr, float xoffset, float yoffset, int32_t size) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_tilt_shift_blur_f021(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(in2), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<uint64_t>(A_fptr), ::taihe::into_abi<uint64_t>(B_fptr), ::taihe::into_abi<uint64_t>(C_fptr), ::taihe::into_abi<float>(xoffset), ::taihe::into_abi<float>(yoffset), ::taihe::into_abi<int32_t>(size)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError radial_blur(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, float sharpness, int32_t strength, float xCenter, float yCenter) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_radial_blur_f02(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<float>(sharpness), ::taihe::into_abi<int32_t>(strength), ::taihe::into_abi<float>(xCenter), ::taihe::into_abi<float>(yCenter)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError rotational_blur(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, float angle, int32_t strength, float x, float y) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_rotational_blur_f02(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<float>(angle), ::taihe::into_abi<int32_t>(strength), ::taihe::into_abi<float>(x), ::taihe::into_abi<float>(y)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError minmax(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t radius, int32_t type) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_minmax_f0(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<int32_t>(radius), ::taihe::into_abi<int32_t>(type)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError color_halftone(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, float cyanAngle, float yellowAngle, float magentaAngle, float radius, uint64_t lookup_fptr) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_color_halftone_f02(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<float>(cyanAngle), ::taihe::into_abi<float>(yellowAngle), ::taihe::into_abi<float>(magentaAngle), ::taihe::into_abi<float>(radius), ::taihe::into_abi<uint64_t>(lookup_fptr)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError sharpen(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, uint64_t kernel_i32ptr, int32_t size) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_sharpen_f0(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<uint64_t>(kernel_i32ptr), ::taihe::into_abi<int32_t>(size)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError polar_coordinates(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t type) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_polar_coordinates_f02(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<int32_t>(type)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError clouds(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, uint64_t permuteLookup_i32ptr, int32_t size, int32_t type) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_clouds_f0(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<uint64_t>(permuteLookup_i32ptr), ::taihe::into_abi<int32_t>(size), ::taihe::into_abi<int32_t>(type)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError motion_blur(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t distance, int32_t angle, float proportion) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_motion_blur_f02(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<int32_t>(distance), ::taihe::into_abi<int32_t>(angle), ::taihe::into_abi<float>(proportion)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError twirl_warp(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t angle) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_twirl_warp_f02(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<int32_t>(angle)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError zigzag(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t wavelength, int32_t amplitude, int32_t type_wave, float proportion) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_zigzag_f0(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<int32_t>(wavelength), ::taihe::into_abi<int32_t>(amplitude), ::taihe::into_abi<int32_t>(type_wave), ::taihe::into_abi<float>(proportion)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError spherize(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t alpha, int32_t type) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_spherize_f0(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<int32_t>(alpha), ::taihe::into_abi<int32_t>(type)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError pinch(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t amount) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_pinch_f0(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<int32_t>(amount)));
}
}
namespace sgl_image {
inline ::sgl_image::SglError wave(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t wavelength, int32_t amplitude, int32_t x_pro, int32_t y_pro, int32_t type, int32_t method) {
    return ::taihe::from_abi<::sgl_image::SglError>(sgl_image_wave_f0(::taihe::into_abi<::sgl_image::SglImageInfo const&>(in), ::taihe::into_abi<::sgl_image::SglImageInfo const&>(out), ::taihe::into_abi<int32_t>(wavelength), ::taihe::into_abi<int32_t>(amplitude), ::taihe::into_abi<int32_t>(x_pro), ::taihe::into_abi<int32_t>(y_pro), ::taihe::into_abi<int32_t>(type), ::taihe::into_abi<int32_t>(method)));
}
}
#pragma clang diagnostic pop
