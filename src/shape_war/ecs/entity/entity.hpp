#pragma once

#include "base_component_t.hpp"
#include "collision_t.hpp"
#include "input_t.hpp"
#include "lifespan_t.hpp"
#include "placement_t.hpp"
#include "score_t.hpp"
#include "shape_t.hpp"

// std includes
#include <string>
#include <tuple>

namespace bv::lab::game::ecs {

using namespace component;
namespace entity {
class Entity_t {

protected:
    using ComponentsAsTuple_t = std::tuple<BaseComponent_t, Placement_t, Shape_t, Collision_t, Score_t, Lifespan_t, Input_t>;

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
}   // namespace entity
}   // namespace bv::lab::game::ecs
