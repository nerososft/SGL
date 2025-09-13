#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "sgl_image.SglBuffer.proj.0.hpp"
#include "sgl_image.SglBuffer.abi.1.h"
#include "sgl_image.SglBufferType.proj.1.hpp"
namespace sgl_image {
struct SglBuffer {
    ::sgl_image::SglBufferType type;
    uint64_t bufHandle_ptr;
    uint64_t memHandle_ptr;
    uint64_t data_ptr;
    uint64_t bufferSize;
};
}
namespace sgl_image {
inline bool operator==(::sgl_image::SglBuffer const& lhs, ::sgl_image::SglBuffer const& rhs) {
    return true && lhs.type == rhs.type && lhs.bufHandle_ptr == rhs.bufHandle_ptr && lhs.memHandle_ptr == rhs.memHandle_ptr && lhs.data_ptr == rhs.data_ptr && lhs.bufferSize == rhs.bufferSize;
}
}
template<> struct ::std::hash<::sgl_image::SglBuffer> {
    size_t operator()(::sgl_image::SglBuffer const& val) const {
        ::std::size_t seed = 0;
        seed ^= ::std::hash<::sgl_image::SglBufferType>()(val.type) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= ::std::hash<uint64_t>()(val.bufHandle_ptr) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= ::std::hash<uint64_t>()(val.memHandle_ptr) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= ::std::hash<uint64_t>()(val.data_ptr) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^= ::std::hash<uint64_t>()(val.bufferSize) + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        return seed;
    }
};
#pragma clang diagnostic pop
