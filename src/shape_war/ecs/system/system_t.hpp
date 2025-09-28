#pragma once

#include "player_t.hpp"
#include "bullet_t.hpp"

namespace bv::lab::game::ecs {

using namespace entity;

namespace system {
struct Spawner_t {
    // Player Creation
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

        createPlayer(entityManager, config);
    }

    static auto createPlayer(auto &entityManager, auto &config) {
        player_t::create(entityManager, config);
    }

    // Bullet Creation
    static auto createBullet(auto &entityManager, auto playerEntity, Vec2f const& mousePosVec) {
        static auto config = BulletConfig_t {
            // BaseConfig_t members
            0,                  // posX
            0,                  // posY
            10,                 // shapeRadius
            10,                 // collisionRadius
            2,                  // outlineThickness
            { 255, 255, 255 },  // fillColor {red, green, blue}
            { 255, 255, 255 },  // outlineColor {red, green, blue}
            // PlayerConfig_t members
            20,                 // speed
            20,                 // shapeVertices
            // BulletConfig_t members
            90                  // lifespan
        };

        createBullet(entityManager, config, playerEntity, mousePosVec);
    }

    static auto createBullet(auto &entityManager, auto &config, auto playerEntity, Vec2f const& mousePosVec) {
        auto playerPosVec = playerEntity->template get<Placement_t>().pos;
        Bullet_t::create(entityManager, config, playerPosVec, mousePosVec);
    }

    // Special weapon creation (in our case non destroyable bullet in all sides)
    static auto createSpecialWeapon(auto &entityManager, auto playerEntity) {
        static auto config = BulletConfig_t {
            // BaseConfig_t members
            0,                  // posX
            0,                  // posY
            10,                 // shapeRadius
            10,                 // collisionRadius
            2,                  // outlineThickness
            { 255, 0, 0 },      // fillColor {red, green, blue}
            { 255, 0, 0 },      // outlineColor {red, green, blue}
            // PlayerConfig_t members
            20,                 // speed
            20,                 // shapeVertices
            // BulletConfig_t members
            90                  // lifespan
        };

        auto &playerPlacement = playerEntity->template get<Placement_t>();
        auto &playerShape = playerEntity->template get<Shape_t>();

        auto angle = 0.0;
        auto bulletCount = playerShape.circle.getPointCount();
        for(auto i = 0; i < bulletCount; ++i) {
            Vec2f velocity = Vec2f::getCoOrdinates(config.speed, angle);
            Bullet_t::create(entityManager, config, "specialBullet", playerPlacement.pos, velocity, 0.0f);
            angle += 360.0/bulletCount;
        }
    }
};

}   // namespace system
}   // bv::lab::game::ecs
