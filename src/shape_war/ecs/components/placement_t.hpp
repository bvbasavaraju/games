#pragma once

#include "base_component_t.hpp"

namespace bv::lab::game::ecs {

struct Placement_t : BaseComponent_t {
    Vec2f pos{0, 0};    // Position
    Vec2f velocity{0, 0};    // Velocity
    float angle = 0;    // Angle to rotate with

    Placement_t() = default;
    Placement_t(Vec2f const& pos_, Vec2f velocity_, float angle_)
    : pos(pos_), velocity(velocity_), angle(angle_) {}
};

}   // namespace bv::lab::game::ecs
