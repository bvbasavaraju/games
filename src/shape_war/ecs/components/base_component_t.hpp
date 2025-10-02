#pragma once

#include "vector2.hpp"

namespace bv::lab::game::ecs::component {

struct BaseComponent_t {
    bool exists{false};
};

template <typename T>
concept IsComponent = std::is_same_v<T, BaseComponent_t> || std::is_base_of_v<BaseComponent_t, T>;

}   // namespace bv::lab::game::ecs::component
