#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "sgl_image.Info.abi.2.h"
#include "sgl_image.SglBuffer.abi.2.h"
#include "sgl_image.SglImageCpuInfo.abi.2.h"
#include "sgl_image.SglImageGpuInfo.abi.2.h"
#include "sgl_image.SglImageInfo.abi.2.h"
#include "taihe/common.h"
TH_EXPORT int sgl_image_gray_f0(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, float r, float g, float b);
TH_EXPORT int sgl_image_threshold_split_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t bright);
TH_EXPORT int sgl_image_gaussian_blur_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t r);
TH_EXPORT int sgl_image_gaussian_blur_float_f021(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t r);
TH_EXPORT int sgl_image_surface_blur_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t r, int32_t th);
TH_EXPORT int sgl_image_distort_glass_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, float scale, float intensity, float zoom);
TH_EXPORT int sgl_image_adjust_saturation_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, float v, float s);
TH_EXPORT int sgl_image_palette_knife_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t r, int32_t s);
TH_EXPORT int sgl_image_hue_equal_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out);
TH_EXPORT int sgl_image_blur_edge_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t r, int32_t s, int32_t kernel_type);
TH_EXPORT int sgl_image_custom_kernel_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, uint64_t k_i32ptr, int32_t radius, int32_t offset, int32_t scale);
TH_EXPORT int sgl_image_color_balance_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, uint64_t adjustP_fptr, uint64_t p_i32ptr, int32_t l);
TH_EXPORT int sgl_image_black_white_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, uint64_t weight_fptr, int32_t wSize);
TH_EXPORT int sgl_image_scale_f0(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t weight, int32_t height, int32_t type);
TH_EXPORT int sgl_image_color_separation_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, float roff, float goff, float boff);
TH_EXPORT int sgl_image_mid_value_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, float radius, float threshold);
TH_EXPORT int sgl_image_path_blur_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, uint64_t vec_fptr, int32_t amount, int32_t width, int32_t height, uint64_t startpos_fptr, uint64_t endpos_fptr, uint64_t startvec_fptr, uint64_t endvec_fptr, int32_t num);
TH_EXPORT int sgl_image_crystallize_f0(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, uint64_t posx_fptr, uint64_t posy_fptr, int32_t n);
TH_EXPORT int sgl_image_rotation_blur_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* in2, struct sgl_image_SglImageInfo_t0 const* out, float x, float y, float a, float b, float ina, float inb, int32_t strength, float angle);
TH_EXPORT int sgl_image_facet_f0(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t radius, int32_t intensitylevel);
TH_EXPORT int sgl_image_accented_edge_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, uint64_t sobelx_i32ptr, uint64_t sobely_i32ptr, int32_t size, int32_t type);
TH_EXPORT int sgl_image_iris_blur_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* in2, struct sgl_image_SglImageInfo_t0 const* out, float x, float y, float a, float b, float ina, float inb, float angle);
TH_EXPORT int sgl_image_tilt_shift_blur_f021(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* in2, struct sgl_image_SglImageInfo_t0 const* out, uint64_t A_fptr, uint64_t B_fptr, uint64_t C_fptr, float xoffset, float yoffset, int32_t size);
TH_EXPORT int sgl_image_radial_blur_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, float sharpness, int32_t strength, float xCenter, float yCenter);
TH_EXPORT int sgl_image_rotational_blur_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, float angle, int32_t strength, float x, float y);
TH_EXPORT int sgl_image_minmax_f0(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t radius, int32_t type);
TH_EXPORT int sgl_image_color_halftone_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, float cyanAngle, float yellowAngle, float magentaAngle, float radius, uint64_t lookup_fptr);
TH_EXPORT int sgl_image_sharpen_f0(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, uint64_t kernel_i32ptr, int32_t size);
TH_EXPORT int sgl_image_polar_coordinates_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t type);
TH_EXPORT int sgl_image_clouds_f0(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, uint64_t permuteLookup_i32ptr, int32_t size, int32_t type);
TH_EXPORT int sgl_image_motion_blur_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t distance, int32_t angle, float proportion);
TH_EXPORT int sgl_image_twirl_warp_f02(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t angle);
TH_EXPORT int sgl_image_zigzag_f0(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t wavelength, int32_t amplitude, int32_t type_wave, float proportion);
TH_EXPORT int sgl_image_spherize_f0(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t alpha, int32_t type);
TH_EXPORT int sgl_image_pinch_f0(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t amount);
TH_EXPORT int sgl_image_wave_f0(struct sgl_image_SglImageInfo_t0 const* in, struct sgl_image_SglImageInfo_t0 const* out, int32_t wavelength, int32_t amplitude, int32_t x_pro, int32_t y_pro, int32_t type, int32_t method);
#pragma clang diagnostic pop
