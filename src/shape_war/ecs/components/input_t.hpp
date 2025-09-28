#pragma once

#include "base_component_t.hpp"

namespace bv::lab::game::ecs::component {

    struct Input_t : BaseComponent_t {
    bool up{false};
    bool down{false};
    bool left{false};
    bool right{false};
    bool shoot{false};  // Mouse click

    Input_t() = default;
};

}   // namespace bv::lab::game::ecs::component
