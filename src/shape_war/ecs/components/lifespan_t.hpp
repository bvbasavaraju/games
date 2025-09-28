#pragma once

#include "base_component_t.hpp"

namespace bv::lab::game::ecs {

struct Lifespan_t : BaseComponent_t {
    int lifespanInFrames{0};
    int remainingLife{0};

    Lifespan_t() = default;
    Lifespan_t(int totalLifespanInFrames)
    : lifespanInFrames(totalLifespanInFrames), remainingLife(totalLifespanInFrames) {}
};

}   // namespace bv::lab::game::ecs
