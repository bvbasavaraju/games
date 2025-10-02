#pragma once

#include "config.hpp"
#include "bullet_t.hpp"
#include "entity_manager.hpp"

namespace bv::lab::game::ecs::entity {

struct SpecialWeapon_t {

    static auto create(auto &entityManager, auto playerEntity) {
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

}   //namespace bv::lab::game::ecs
