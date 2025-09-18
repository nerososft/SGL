#pragma once
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Weverything"
#pragma clang diagnostic warning "-Wextra"
#pragma clang diagnostic warning "-Wall"
#include "sgl_image.Info.abi.1.h"
#include "sgl_image.Info.proj.0.hpp"
#include "sgl_image.SglImageCpuInfo.proj.1.hpp"
#include "sgl_image.SglImageGpuInfo.proj.1.hpp"
namespace sgl_image {
struct Info {
    public:
    enum class tag_t : int {
        gpu,
        cpu,
    };
    union storage_t {
        storage_t() {}
        ~storage_t() {}
        ::sgl_image::SglImageGpuInfo gpu;
        ::sgl_image::SglImageCpuInfo cpu;
    };
    Info(Info const& other) : m_tag(other.m_tag) {
        switch (m_tag) {
        case tag_t::gpu: {
            new (&m_data.gpu) decltype(m_data.gpu)(other.m_data.gpu);
            break;
        }
        case tag_t::cpu: {
            new (&m_data.cpu) decltype(m_data.cpu)(other.m_data.cpu);
            break;
        }
        default: {
            break;
        }
        }
    }
    Info(Info&& other) : m_tag(other.m_tag) {
        switch (m_tag) {
        case tag_t::gpu: {
            new (&m_data.gpu) decltype(m_data.gpu)(::std::move(other.m_data.gpu));
            break;
        }
        case tag_t::cpu: {
            new (&m_data.cpu) decltype(m_data.cpu)(::std::move(other.m_data.cpu));
            break;
        }
        default: {
            break;
        }
        }
    }
    Info& operator=(Info const& other) {
        if (this != &other) {
            ::std::destroy_at(this);
            new (this) Info(other);
        }
        return *this;
    }
    Info& operator=(Info&& other) {
        if (this != &other) {
            ::std::destroy_at(this);
            new (this) Info(::std::move(other));
        }
        return *this;
    }
    ~Info() {
        switch (m_tag) {
        case tag_t::gpu: {
            ::std::destroy_at(&m_data.gpu);
            break;
        }
        case tag_t::cpu: {
            ::std::destroy_at(&m_data.cpu);
            break;
        }
        default: {
            break;
        }
        }
    }
    template<typename... Args>
    Info(::taihe::static_tag_t<tag_t::gpu>, Args&&... args) : m_tag(tag_t::gpu) {
        new (&m_data.gpu) decltype(m_data.gpu)(::std::forward<Args>(args)...);
    }
    template<typename... Args>
    Info(::taihe::static_tag_t<tag_t::cpu>, Args&&... args) : m_tag(tag_t::cpu) {
        new (&m_data.cpu) decltype(m_data.cpu)(::std::forward<Args>(args)...);
    }
    template<tag_t tag, typename... Args>
    static Info make(Args&&... args) {
        return Info(::taihe::static_tag<tag>, ::std::forward<Args>(args)...);
    }
    template<tag_t tag, typename... Args>
    Info const& emplace(Args&&... args) {
        ::std::destroy_at(this);
        new (this) Info(::taihe::static_tag<tag>, ::std::forward<Args>(args)...);
        return *this;
    }
    tag_t get_tag() const {
        return m_tag;
    }
    template<tag_t tag>
    bool holds() const {
        return m_tag == tag;
    }
    template<tag_t tag>
    auto& get_ref() {
        if constexpr (tag == tag_t::gpu) {
            return m_data.gpu;
        }
        if constexpr (tag == tag_t::cpu) {
            return m_data.cpu;
        }
    }
    template<tag_t tag>
    auto* get_ptr() {
        return m_tag == tag ? &get_ref<tag>() : nullptr;
    }
    template<typename Visitor>
    decltype(auto) match_function(Visitor&& visitor) {
        switch (m_tag) {
        case tag_t::gpu: {
            return visitor(::taihe::static_tag<tag_t::gpu>, m_data.gpu);
        }
        case tag_t::cpu: {
            return visitor(::taihe::static_tag<tag_t::cpu>, m_data.cpu);
        }
        }
    }
    template<typename ReturnType, typename Visitor>
    ReturnType match_function(Visitor&& visitor) {
        switch (m_tag) {
        case tag_t::gpu: {
            return visitor(::taihe::static_tag<tag_t::gpu>, m_data.gpu);
        }
        case tag_t::cpu: {
            return visitor(::taihe::static_tag<tag_t::cpu>, m_data.cpu);
        }
        }
    }
    template<tag_t tag>
    auto const& get_ref() const {
        if constexpr (tag == tag_t::gpu) {
            return m_data.gpu;
        }
        if constexpr (tag == tag_t::cpu) {
            return m_data.cpu;
        }
    }
    template<tag_t tag>
    auto const* get_ptr() const {
        return m_tag == tag ? &get_ref<tag>() : nullptr;
    }
    template<typename Visitor>
    decltype(auto) match_function(Visitor&& visitor) const {
        switch (m_tag) {
        case tag_t::gpu: {
            return visitor(::taihe::static_tag<tag_t::gpu>, m_data.gpu);
        }
        case tag_t::cpu: {
            return visitor(::taihe::static_tag<tag_t::cpu>, m_data.cpu);
        }
        }
    }
    template<typename ReturnType, typename Visitor>
    ReturnType match_function(Visitor&& visitor) const {
        switch (m_tag) {
        case tag_t::gpu: {
            return visitor(::taihe::static_tag<tag_t::gpu>, m_data.gpu);
        }
        case tag_t::cpu: {
            return visitor(::taihe::static_tag<tag_t::cpu>, m_data.cpu);
        }
        }
    }
    template<typename... Args>
    static Info make_gpu(Args&&... args) {
        return make<tag_t::gpu>(::std::forward<Args>(args)...);
    }
    template<typename... Args>
    static Info make_cpu(Args&&... args) {
        return make<tag_t::cpu>(::std::forward<Args>(args)...);
    }
    template<typename... Args>
    Info const& emplace_gpu(Args&&... args) {
        return emplace<tag_t::gpu>(::std::forward<Args>(args)...);
    }
    template<typename... Args>
    Info const& emplace_cpu(Args&&... args) {
        return emplace<tag_t::cpu>(::std::forward<Args>(args)...);
    }
    bool holds_gpu() const {
        return holds<tag_t::gpu>();
    }
    bool holds_cpu() const {
        return holds<tag_t::cpu>();
    }
    auto* get_gpu_ptr() {
        return get_ptr<tag_t::gpu>();
    }
    auto* get_cpu_ptr() {
        return get_ptr<tag_t::cpu>();
    }
    auto& get_gpu_ref() {
        return get_ref<tag_t::gpu>();
    }
    auto& get_cpu_ref() {
        return get_ref<tag_t::cpu>();
    }
    template<typename Visitor>
    decltype(auto) match(Visitor&& visitor) {
        switch (m_tag) {
        case tag_t::gpu: {
            return visitor.case_gpu(m_data.gpu);
        }
        case tag_t::cpu: {
            return visitor.case_cpu(m_data.cpu);
        }
        }
    }
    template<typename ReturnType, typename Visitor>
    ReturnType match(Visitor&& visitor) {
        switch (m_tag) {
        case tag_t::gpu: {
            return visitor.case_gpu(m_data.gpu);
        }
        case tag_t::cpu: {
            return visitor.case_cpu(m_data.cpu);
        }
        }
    }
    auto const* get_gpu_ptr() const {
        return get_ptr<tag_t::gpu>();
    }
    auto const* get_cpu_ptr() const {
        return get_ptr<tag_t::cpu>();
    }
    auto const& get_gpu_ref() const {
        return get_ref<tag_t::gpu>();
    }
    auto const& get_cpu_ref() const {
        return get_ref<tag_t::cpu>();
    }
    template<typename Visitor>
    decltype(auto) match(Visitor&& visitor) const {
        switch (m_tag) {
        case tag_t::gpu: {
            return visitor.case_gpu(m_data.gpu);
        }
        case tag_t::cpu: {
            return visitor.case_cpu(m_data.cpu);
        }
        }
    }
    template<typename ReturnType, typename Visitor>
    ReturnType match(Visitor&& visitor) const {
        switch (m_tag) {
        case tag_t::gpu: {
            return visitor.case_gpu(m_data.gpu);
        }
        case tag_t::cpu: {
            return visitor.case_cpu(m_data.cpu);
        }
        }
    }
    private:
    tag_t m_tag;
    storage_t m_data;
};
}
namespace sgl_image {
inline bool operator==(::sgl_image::Info const& lhs, ::sgl_image::Info const& rhs) {
    return false || (lhs.holds_gpu() && rhs.holds_gpu() && lhs.get_gpu_ref() == rhs.get_gpu_ref()) || (lhs.holds_cpu() && rhs.holds_cpu() && lhs.get_cpu_ref() == rhs.get_cpu_ref());
}
}
template<> struct ::std::hash<::sgl_image::Info> {
    size_t operator()(::sgl_image::Info const& val) const {
        switch (val.get_tag()) {
        case ::sgl_image::Info::tag_t::gpu: {
            ::std::size_t seed = ::std::hash<int>()(static_cast<int>(::sgl_image::Info::tag_t::gpu));
            return seed ^ (0x9e3779b9 + (seed << 6) + (seed >> 2) + ::std::hash<::sgl_image::SglImageGpuInfo>()(val.get_gpu_ref()));
        }
        case ::sgl_image::Info::tag_t::cpu: {
            ::std::size_t seed = ::std::hash<int>()(static_cast<int>(::sgl_image::Info::tag_t::cpu));
            return seed ^ (0x9e3779b9 + (seed << 6) + (seed >> 2) + ::std::hash<::sgl_image::SglImageCpuInfo>()(val.get_cpu_ref()));
        }
        }
    }
};
#pragma clang diagnostic pop
