#pragma once

#include "base_component_t.hpp"

namespace bv::lab::game::ecs {

struct Collision_t : BaseComponent_t {
    float radius{0.0};

    Collision_t() = default;
    Collision_t(float r) 
    : radius(r) {}
};

}   // namespace bv::lab::game::ecs
