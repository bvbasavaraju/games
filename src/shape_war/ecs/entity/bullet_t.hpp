#pragma once

#include "config.hpp"
#include "entity_manager.hpp"

namespace bv::lab::game::ecs::entity {

struct Bullet_t {

    static auto create(auto &entityManager, auto &config, auto &srcPosVec, auto &destPosVec) {
        // Distance vector
        auto distVec = destPosVec - srcPosVec;
        distVec.normalize();

        // Bullet speed and velocity
        auto bulletVelocity = Vec2f{distVec.x * config.speed, distVec.y * config.speed};
        float bulletAngle = distVec.angle();
        
        // Create Bullet entity
        create(entityManager, config, "bullet", srcPosVec, bulletVelocity, bulletAngle);
    }

    static auto create(auto &entityManager, auto &config, auto const& tag, auto &pos, auto &velocity, auto angle) {
        auto bullet = entityManager.addEntity(tag);
        bullet->template add<Placement_t>(pos, velocity, angle);

        bullet->template add<Shape_t>(
            config.shapeRadius,
            config.shapeVertices,
            sf::Color{config.fillColor.red, config.fillColor.green, config.fillColor.blue},
            sf::Color{config.outlineColor.red, config.outlineColor.green, config.outlineColor.blue},
            config.outlineThickness);
        
        bullet->template add<Lifespan_t>(config.lifespan);
        
        bullet->template add<Collision_t>(config.collisionRadius);
    }
};

}   //namespace bv::lab::game::ecs
