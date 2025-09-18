#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "sgl_image.SglBufferType.proj.0.hpp"
namespace sgl_image {
struct SglBufferType {
    public:
    enum class key_t: int {
        SGL_BUFFER_TYPE_UNIFORM,
        SGL_BUFFER_TYPE_STORAGE_LOCAL,
        SGL_BUFFER_TYPE_STORAGE_SHARED,
        SGL_BUFFER_TYPE_VERTEX,
        SGL_BUFFER_TYPE_INDEX,
        SGL_BUFFER_TYPE_UNKNOWN,
    };
    SglBufferType(SglBufferType const& other) : key(other.key) {}
    SglBufferType& operator=(SglBufferType other) {
        key = other.key;
        return *this;
    }
    SglBufferType(key_t key) : key(key) {}
    key_t get_key() const {
        return key;
    }
    bool is_valid() const {
        return static_cast<int>(key) >= 0 && static_cast<int>(key) < 6;
    }
    static constexpr int32_t table[] = {
        0,
        1,
        2,
        3,
        4,
        5,
    };
    int32_t get_value() const {
        return table[static_cast<int>(key)];
    }
    operator int32_t() const {
        return table[static_cast<int>(key)];
    }
    static ::sgl_image::SglBufferType from_value(int32_t value) {
        if (value == 0) {
            return ::sgl_image::SglBufferType(static_cast<key_t>(0));
        }
        if (value == 1) {
            return ::sgl_image::SglBufferType(static_cast<key_t>(1));
        }
        if (value == 2) {
            return ::sgl_image::SglBufferType(static_cast<key_t>(2));
        }
        if (value == 3) {
            return ::sgl_image::SglBufferType(static_cast<key_t>(3));
        }
        if (value == 4) {
            return ::sgl_image::SglBufferType(static_cast<key_t>(4));
        }
        if (value == 5) {
            return ::sgl_image::SglBufferType(static_cast<key_t>(5));
        }
        return ::sgl_image::SglBufferType(static_cast<key_t>(-1));
    }
    private:
    key_t key;
};
}
namespace sgl_image {
inline bool operator==(::sgl_image::SglBufferType lhs, ::sgl_image::SglBufferType rhs) {
    return lhs.get_key() == rhs.get_key();
}
}
template<> struct ::std::hash<::sgl_image::SglBufferType> {
    size_t operator()(::sgl_image::SglBufferType val) const {
        return ::std::hash<int>()(static_cast<int>(val.get_key()));
    }
};
#pragma clang diagnostic pop
