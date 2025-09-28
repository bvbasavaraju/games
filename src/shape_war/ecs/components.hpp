#pragma once

#include "../utils/vector2.hpp"

namespace bv::lab {

struct Component_t {
    bool exists{false};
};

struct Placement_t : Component_t{
    Vec2f pos{0, 0};    // Position
    Vec2f velocity{0, 0};    // Velocity
    float angle = 0;    // Angle to rotate with

    Placement_t() = default;
    Placement_t(Vec2f const& pos_, Vec2f velocity_, float angle_)
    : pos(pos_), velocity(velocity_), angle(angle_) {}
};

struct Shape_t : Component_t {
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

struct Collision_t : Component_t {
    float radius{0.0};

    Collision_t() = default;
    Collision_t(float r) 
    : radius(r) {}
};

struct Score_t : Component_t {
    int score{0};

    Score_t() = default;
    Score_t(int s)
    : score(s) {}
};

struct Lifespan_t : Component_t {
    int lifespanInFrames{0};
    int remainingLife{0};

    Lifespan_t() = default;
    Lifespan_t(int totalLifespanInFrames)
    : lifespanInFrames(totalLifespanInFrames), remainingLife(totalLifespanInFrames) {}
};

struct Input_t : Component_t {
    bool up{false};
    bool down{false};
    bool left{false};
    bool right{false};
    bool shoot{false};  // Mouse click

    Input_t() = default;
};

template <typename T>
concept IsComponent = std::is_same_v<T, Component_t> || std::is_base_of_v<Component_t, T>;

using ComponentsAsTuple_t = std::tuple<Component_t, Placement_t, Shape_t, Collision_t, Score_t, Lifespan_t, Input_t>;
}   // namespace bv::lab
