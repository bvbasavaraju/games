#pragma once

#include "player_t.hpp"

namespace bv::lab::game::ecs {

using namespace entity;

namespace system {
struct Spawner_t {
    static auto createPlayer(auto &entityManager, auto screenWidth, auto screenHeight) {
        static auto config = PlayerConfig_t {
            // BaseConfig_t members
            screenWidth >> 1,    // posX
            screenHeight >> 1,   // posY
            32,                  // shapeRadius
            32,                  // collisionRadius
            4,                   // outlineThickness
            { 5, 5, 5 },        // fillColor {red, green, blue}
            { 255, 0, 0 },      // outlineColor {red, green, blue}
            // PlayerConfig_t members
            1,                   // speed
            8                    // shapeVertices
        };
        player_t::create(entityManager, config);
    }

    static auto createPlayer(auto &entityManager, auto &config) {
        player_t::create(entityManager, config);
    }
};

}   // namespace system
}   // bv::lab::game::ecs
