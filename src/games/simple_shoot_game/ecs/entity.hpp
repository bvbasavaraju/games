#pragma once

#include "components.hpp"

// std includes
#include <string>
#include <tuple>

namespace bv::lab {

class Entity_t {

private:
    ComponentsAsTuple_t mComponents{};
    bool mActive{true};
    std::string mTag = "None";    // TODO: Convert this to type and pass it as template argument
    std::size_t mId{0};

public:
    Entity_t() = default;
    Entity_t(std::size_t id_, std::string const& tag_)
    : mId(id_), mTag(tag_) {}

    auto isActive() const -> bool {
        return mActive;
    } 

    auto destroy() -> void {
        mActive = false;
    }

    auto id() -> std::size_t {
        return mId;
    }

    auto tag() const -> std::string const&  {
        return mTag;
    }

    template <IsComponent T>
    auto has() const -> bool {
        return get<T>().exists;
    }

    template <IsComponent T>
    auto get() -> T& {
        return std::get<T>(mComponents);
    }

    template <IsComponent T>
    auto get() const -> T const& {
        return std::get<T>(mComponents);
    }

    template <IsComponent T, typename ...Args>
    auto add(Args... args) -> T& {
        auto& comp = get<T>();
        comp = T(std::forward<Args>(args)...);
        comp.exists = true;
        return comp;
    }

    template <typename T>
    auto remove() -> void {
        auto& comp = get<T>();
        comp = T();
    }
};
}   // namespace bv::lab
