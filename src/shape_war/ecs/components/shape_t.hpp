#pragma once

#include "base_component_t.hpp"

namespace bv::lab::game::ecs::component {

struct Shape_t : BaseComponent_t {
    sf::CircleShape circle;

    Shape_t() = default;
    Shape_t(float radius, std::size_t circlePoints, sf::Color const& fillColor, sf::Color const& outlineColor, float thickness)
    : circle(radius, circlePoints) {
        circle.setFillColor(fillColor);
        circle.setOutlineColor(outlineColor);
        circle.setOutlineThickness(thickness);
        circle.setOrigin({radius, radius});
    }
};

}   // namespace bv::lab::game::ecs::component
