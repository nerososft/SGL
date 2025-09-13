#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "sgl_image.SglImageCpuInfo.proj.0.hpp"
#include "sgl_image.SglImageCpuInfo.abi.1.h"
namespace sgl_image {
struct SglImageCpuInfo {
    uint32_t width;
    uint32_t height;
    uint32_t channels;
    uint32_t bytesPerLine;
    uint64_t data_ptr;
};
}
namespace sgl_image {
inline bool operator==(::sgl_image::SglImageCpuInfo const& lhs, ::sgl_image::SglImageCpuInfo const& rhs) {
    return true && lhs.width == rhs.width && lhs.height == rhs.height && lhs.channels == rhs.channels && lhs.bytesPerLine == rhs.bytesPerLine && lhs.data_ptr == rhs.data_ptr;
}
}
template<> struct ::std::hash<::sgl_image::SglImageCpuInfo> {
    size_t operator()(::sgl_image::SglImageCpuInfo const& val) const {
        ::std::size_t seed = 0;
        seed ^= ::std::hash<uint32_t>()(val.width) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= ::std::hash<uint32_t>()(val.height) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= ::std::hash<uint32_t>()(val.channels) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= ::std::hash<uint32_t>()(val.bytesPerLine) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= ::std::hash<uint64_t>()(val.data_ptr) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
};
#pragma clang diagnostic pop
