#pragma once

#include "base_component_t.hpp"

namespace bv::lab::game::ecs {

struct Score_t : BaseComponent_t {
    int score{0};

    Score_t() = default;
    Score_t(int s)
    : score(s) {}
};

}   // namespace bv::lab::game::ecs
