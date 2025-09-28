#pragma once

#include "config.hpp"
#include "entity_manager.hpp"

namespace bv::lab::game::ecs::entity {

struct Player_t {

    static auto create(auto &entityManager, auto &config) {
        auto player = entityManager.addEntity("player");
        player->template add<Placement_t>(
            Vec2f{config.posX * 1.0f, config.posY * 1.0f}, 
            Vec2f{config.speed * 1.0f, config.speed * 1.0f}, 
            0.0);
        
        player->template add<Shape_t>(
            config.shapeRadius, 
            config.shapeVertices, 
            sf::Color{config.fillColor.red, config.fillColor.green, config.fillColor.blue},
            sf::Color{config.outlineColor.red, config.outlineColor.green, config.outlineColor.blue},
            config.outlineThickness);

        player->template add<Input_t>();
    }
};

}   //namespace bv::lab::game::ecs
