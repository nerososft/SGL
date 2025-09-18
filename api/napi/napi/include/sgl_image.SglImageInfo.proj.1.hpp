#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "sgl_image.Info.proj.1.hpp"
#include "sgl_image.SglImageInfo.abi.1.h"
#include "sgl_image.SglImageInfo.proj.0.hpp"
#include "sgl_image.SglImageType.proj.1.hpp"
namespace sgl_image {
struct SglImageInfo {
    ::sgl_image::SglImageType type;
    ::sgl_image::Info info;
};
}
namespace sgl_image {
inline bool operator==(::sgl_image::SglImageInfo const& lhs, ::sgl_image::SglImageInfo const& rhs) {
    return true && lhs.type == rhs.type && lhs.info == rhs.info;
}
}
template<> struct ::std::hash<::sgl_image::SglImageInfo> {
    size_t operator()(::sgl_image::SglImageInfo const& val) const {
        ::std::size_t seed = 0;
        seed ^= ::std::hash<::sgl_image::SglImageType>()(val.type) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= ::std::hash<::sgl_image::Info>()(val.info) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
};
#pragma clang diagnostic pop
