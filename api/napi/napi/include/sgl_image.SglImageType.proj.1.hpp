#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "sgl_image.SglImageType.proj.0.hpp"
namespace sgl_image {
struct SglImageType {
    public:
    enum class key_t: int {
        SGL_IMAGE_TYPE_CPU,
        SGL_IMAGE_TYPE_GPU,
        SGL_IMAGE_TYPE_UNKNOWN,
    };
    SglImageType(SglImageType const& other) : key(other.key) {}
    SglImageType& operator=(SglImageType other) {
        key = other.key;
        return *this;
    }
    SglImageType(key_t key) : key(key) {}
    key_t get_key() const {
        return key;
    }
    bool is_valid() const {
        return static_cast<int>(key) >= 0 && static_cast<int>(key) < 3;
    }
    static constexpr int32_t table[] = {
        0,
        1,
        2,
    };
    int32_t get_value() const {
        return table[static_cast<int>(key)];
    }
    operator int32_t() const {
        return table[static_cast<int>(key)];
    }
    static ::sgl_image::SglImageType from_value(int32_t value) {
        if (value == 0) {
            return ::sgl_image::SglImageType(static_cast<key_t>(0));
        }
        if (value == 1) {
            return ::sgl_image::SglImageType(static_cast<key_t>(1));
        }
        if (value == 2) {
            return ::sgl_image::SglImageType(static_cast<key_t>(2));
        }
        return ::sgl_image::SglImageType(static_cast<key_t>(-1));
    }
    private:
    key_t key;
};
}
namespace sgl_image {
inline bool operator==(::sgl_image::SglImageType lhs, ::sgl_image::SglImageType rhs) {
    return lhs.get_key() == rhs.get_key();
}
}
template<> struct ::std::hash<::sgl_image::SglImageType> {
    size_t operator()(::sgl_image::SglImageType val) const {
        return ::std::hash<int>()(static_cast<int>(val.get_key()));
    }
};
#pragma clang diagnostic pop
