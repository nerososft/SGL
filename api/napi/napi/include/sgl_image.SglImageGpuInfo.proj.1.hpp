#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "sgl_image.SglBuffer.proj.1.hpp"
#include "sgl_image.SglImageGpuInfo.abi.1.h"
#include "sgl_image.SglImageGpuInfo.proj.0.hpp"
namespace sgl_image {
struct SglImageGpuInfo {
    uint32_t width;
    uint32_t height;
    uint32_t channels;
    uint32_t bytesPerLine;
    ::sgl_image::SglBuffer gpuBuf;
};
}
namespace sgl_image {
inline bool operator==(::sgl_image::SglImageGpuInfo const& lhs, ::sgl_image::SglImageGpuInfo const& rhs) {
    return true && lhs.width == rhs.width && lhs.height == rhs.height && lhs.channels == rhs.channels && lhs.bytesPerLine == rhs.bytesPerLine && lhs.gpuBuf == rhs.gpuBuf;
}
}
template<> struct ::std::hash<::sgl_image::SglImageGpuInfo> {
    size_t operator()(::sgl_image::SglImageGpuInfo const& val) const {
        ::std::size_t seed = 0;
        seed ^= ::std::hash<uint32_t>()(val.width) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= ::std::hash<uint32_t>()(val.height) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= ::std::hash<uint32_t>()(val.channels) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= ::std::hash<uint32_t>()(val.bytesPerLine) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= ::std::hash<::sgl_image::SglBuffer>()(val.gpuBuf) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
};
#pragma clang diagnostic pop
