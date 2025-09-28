#pragma once

#include <cstdint>

namespace bv::lab::game::ecs::entity {

struct Color_t {
    std::uint8_t red;
    std::uint8_t green;
    std::uint8_t blue;
};

struct BaseConfig_t {
    std::uint32_t posX;
    std::uint32_t posY;
    int shapeRadius;
    int collisionRadius;
    int outlineThickness;
    Color_t fillColor;
    Color_t outlineColor;
};

struct PlayerConfig_t : BaseConfig_t {
    int speed;
    int shapeVertices;
};

struct EnemyConfig_t : BaseConfig_t {
    float speedMin;
    float speedMax;
    int verticesMin;
    int verticesMax;
    int lifespan;
    int spawnInterval;
};

struct BulletConfig_t : PlayerConfig_t {
    int lifespan;
};

}   // namespace bv::lab::game::ecs::entity
