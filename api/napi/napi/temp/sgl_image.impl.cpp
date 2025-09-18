#include "sgl_image.proj.hpp"
#include "sgl_image.impl.hpp"
#include "taihe/runtime.hpp"
#include "stdexcept"


namespace {
// To be implemented.

::sgl_image::SglError gray(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, float r, float g, float b) {
    TH_THROW(std::runtime_error, "gray not implemented");
}

::sgl_image::SglError threshold_split(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t bright) {
    TH_THROW(std::runtime_error, "threshold_split not implemented");
}

::sgl_image::SglError gaussian_blur(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t r) {
    TH_THROW(std::runtime_error, "gaussian_blur not implemented");
}

::sgl_image::SglError gaussian_blur_float(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t r) {
    TH_THROW(std::runtime_error, "gaussian_blur_float not implemented");
}

::sgl_image::SglError surface_blur(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t r, int32_t th) {
    TH_THROW(std::runtime_error, "surface_blur not implemented");
}

::sgl_image::SglError distort_glass(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, float scale, float intensity, float zoom) {
    TH_THROW(std::runtime_error, "distort_glass not implemented");
}

::sgl_image::SglError adjust_saturation(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, float v, float s) {
    TH_THROW(std::runtime_error, "adjust_saturation not implemented");
}

::sgl_image::SglError palette_knife(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t r, int32_t s) {
    TH_THROW(std::runtime_error, "palette_knife not implemented");
}

::sgl_image::SglError hue_equal(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out) {
    TH_THROW(std::runtime_error, "hue_equal not implemented");
}

::sgl_image::SglError blur_edge(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t r, int32_t s, int32_t kernel_type) {
    TH_THROW(std::runtime_error, "blur_edge not implemented");
}

::sgl_image::SglError custom_kernel(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, uint64_t k_i32ptr, int32_t radius, int32_t offset, int32_t scale) {
    TH_THROW(std::runtime_error, "custom_kernel not implemented");
}

::sgl_image::SglError color_balance(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, uint64_t adjustP_fptr, uint64_t p_i32ptr, int32_t l) {
    TH_THROW(std::runtime_error, "color_balance not implemented");
}

::sgl_image::SglError black_white(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, uint64_t weight_fptr, int32_t wSize) {
    TH_THROW(std::runtime_error, "black_white not implemented");
}

::sgl_image::SglError scale(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t weight, int32_t height, int32_t type) {
    TH_THROW(std::runtime_error, "scale not implemented");
}

::sgl_image::SglError color_separation(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, float roff, float goff, float boff) {
    TH_THROW(std::runtime_error, "color_separation not implemented");
}

::sgl_image::SglError mid_value(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, float radius, float threshold) {
    TH_THROW(std::runtime_error, "mid_value not implemented");
}

::sgl_image::SglError path_blur(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, uint64_t vec_fptr, int32_t amount, int32_t width, int32_t height, uint64_t startpos_fptr, uint64_t endpos_fptr, uint64_t startvec_fptr, uint64_t endvec_fptr, int32_t num) {
    TH_THROW(std::runtime_error, "path_blur not implemented");
}

::sgl_image::SglError crystallize(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, uint64_t posx_fptr, uint64_t posy_fptr, int32_t n) {
    TH_THROW(std::runtime_error, "crystallize not implemented");
}

::sgl_image::SglError rotation_blur(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& in2, ::sgl_image::SglImageInfo const& out, float x, float y, float a, float b, float ina, float inb, int32_t strength, float angle) {
    TH_THROW(std::runtime_error, "rotation_blur not implemented");
}

::sgl_image::SglError facet(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t radius, int32_t intensitylevel) {
    TH_THROW(std::runtime_error, "facet not implemented");
}

::sgl_image::SglError accented_edge(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, uint64_t sobelx_i32ptr, uint64_t sobely_i32ptr, int32_t size, int32_t type) {
    TH_THROW(std::runtime_error, "accented_edge not implemented");
}

::sgl_image::SglError iris_blur(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& in2, ::sgl_image::SglImageInfo const& out, float x, float y, float a, float b, float ina, float inb, float angle) {
    TH_THROW(std::runtime_error, "iris_blur not implemented");
}

::sgl_image::SglError tilt_shift_blur(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& in2, ::sgl_image::SglImageInfo const& out, uint64_t A_fptr, uint64_t B_fptr, uint64_t C_fptr, float xoffset, float yoffset, int32_t size) {
    TH_THROW(std::runtime_error, "tilt_shift_blur not implemented");
}

::sgl_image::SglError radial_blur(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, float sharpness, int32_t strength, float xCenter, float yCenter) {
    TH_THROW(std::runtime_error, "radial_blur not implemented");
}

::sgl_image::SglError rotational_blur(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, float angle, int32_t strength, float x, float y) {
    TH_THROW(std::runtime_error, "rotational_blur not implemented");
}

::sgl_image::SglError minmax(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t radius, int32_t type) {
    TH_THROW(std::runtime_error, "minmax not implemented");
}

::sgl_image::SglError color_halftone(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, float cyanAngle, float yellowAngle, float magentaAngle, float radius, uint64_t lookup_fptr) {
    TH_THROW(std::runtime_error, "color_halftone not implemented");
}

::sgl_image::SglError sharpen(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, uint64_t kernel_i32ptr, int32_t size) {
    TH_THROW(std::runtime_error, "sharpen not implemented");
}

::sgl_image::SglError polar_coordinates(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t type) {
    TH_THROW(std::runtime_error, "polar_coordinates not implemented");
}

::sgl_image::SglError clouds(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, uint64_t permuteLookup_i32ptr, int32_t size, int32_t type) {
    TH_THROW(std::runtime_error, "clouds not implemented");
}

::sgl_image::SglError motion_blur(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t distance, int32_t angle, float proportion) {
    TH_THROW(std::runtime_error, "motion_blur not implemented");
}

::sgl_image::SglError twirl_warp(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t angle) {
    TH_THROW(std::runtime_error, "twirl_warp not implemented");
}

::sgl_image::SglError zigzag(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t wavelength, int32_t amplitude, int32_t type_wave, float proportion) {
    TH_THROW(std::runtime_error, "zigzag not implemented");
}

::sgl_image::SglError spherize(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t alpha, int32_t type) {
    TH_THROW(std::runtime_error, "spherize not implemented");
}

::sgl_image::SglError pinch(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t amount) {
    TH_THROW(std::runtime_error, "pinch not implemented");
}

::sgl_image::SglError wave(::sgl_image::SglImageInfo const& in, ::sgl_image::SglImageInfo const& out, int32_t wavelength, int32_t amplitude, int32_t x_pro, int32_t y_pro, int32_t type, int32_t method) {
    TH_THROW(std::runtime_error, "wave not implemented");
}
}  // namespace

// Since these macros are auto-generate, lint will cause false positive.
// NOLINTBEGIN
TH_EXPORT_CPP_API_gray(gray);
TH_EXPORT_CPP_API_threshold_split(threshold_split);
TH_EXPORT_CPP_API_gaussian_blur(gaussian_blur);
TH_EXPORT_CPP_API_gaussian_blur_float(gaussian_blur_float);
TH_EXPORT_CPP_API_surface_blur(surface_blur);
TH_EXPORT_CPP_API_distort_glass(distort_glass);
TH_EXPORT_CPP_API_adjust_saturation(adjust_saturation);
TH_EXPORT_CPP_API_palette_knife(palette_knife);
TH_EXPORT_CPP_API_hue_equal(hue_equal);
TH_EXPORT_CPP_API_blur_edge(blur_edge);
TH_EXPORT_CPP_API_custom_kernel(custom_kernel);
TH_EXPORT_CPP_API_color_balance(color_balance);
TH_EXPORT_CPP_API_black_white(black_white);
TH_EXPORT_CPP_API_scale(scale);
TH_EXPORT_CPP_API_color_separation(color_separation);
TH_EXPORT_CPP_API_mid_value(mid_value);
TH_EXPORT_CPP_API_path_blur(path_blur);
TH_EXPORT_CPP_API_crystallize(crystallize);
TH_EXPORT_CPP_API_rotation_blur(rotation_blur);
TH_EXPORT_CPP_API_facet(facet);
TH_EXPORT_CPP_API_accented_edge(accented_edge);
TH_EXPORT_CPP_API_iris_blur(iris_blur);
TH_EXPORT_CPP_API_tilt_shift_blur(tilt_shift_blur);
TH_EXPORT_CPP_API_radial_blur(radial_blur);
TH_EXPORT_CPP_API_rotational_blur(rotational_blur);
TH_EXPORT_CPP_API_minmax(minmax);
TH_EXPORT_CPP_API_color_halftone(color_halftone);
TH_EXPORT_CPP_API_sharpen(sharpen);
TH_EXPORT_CPP_API_polar_coordinates(polar_coordinates);
TH_EXPORT_CPP_API_clouds(clouds);
TH_EXPORT_CPP_API_motion_blur(motion_blur);
TH_EXPORT_CPP_API_twirl_warp(twirl_warp);
TH_EXPORT_CPP_API_zigzag(zigzag);
TH_EXPORT_CPP_API_spherize(spherize);
TH_EXPORT_CPP_API_pinch(pinch);
TH_EXPORT_CPP_API_wave(wave);
// NOLINTEND
