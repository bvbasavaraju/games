#pragma once

#include "config.hpp"
#include "entity_manager.hpp"

namespace bv::lab::game::ecs::entity {

struct Enemy_t {

    static auto create(auto &entityManager, auto &config, auto &posVec, auto &velocityVec) {
        static std::array<sf::Color, 6> fillColors = {
            sf::Color::Red, 
            sf::Color::Green,
            sf::Color::Blue,
            sf::Color::Yellow,
            sf::Color::Magenta,
            sf::Color::Cyan };

        auto edges = rand() % (config.verticesMax - config.verticesMin + 1);
        auto vertices = edges + config.verticesMin;   // Offset of 3 vertices

        auto fillColor = fillColors[edges % fillColors.size()];
        fillColor.a = 150;

        // add necessary components
        auto enemy = entityManager.addEntity("enemy");
        enemy->template add<Placement_t>(posVec, velocityVec, 0.0);
        enemy->template add<Shape_t>(
            config.shapeRadius,
            vertices,
            fillColor,
            sf::Color{config.outlineColor.red, config.outlineColor.green, config.outlineColor.blue},
            config.outlineThickness
        );
        enemy->template add<Collision_t>(config.collisionRadius);
        enemy->template add<Score_t>((vertices - config.verticesMin + 1) * 10);
        enemy->template add<Lifespan_t>(config.lifespan);
    }

    static auto createSmaller(auto &entityManager, auto parentEnemy) {
        auto &enemyPlacement = parentEnemy->template get<Placement_t>();
        auto &enemyShape = parentEnemy->template get<Shape_t>();
        auto &enemyCollision = parentEnemy->template get<Collision_t>();
        auto &enemyScore = parentEnemy->template get<Score_t>();
        auto &enemyLifespan = parentEnemy->template get<Lifespan_t>();

        auto fillColor = enemyShape.circle.getFillColor();
        fillColor.a >>= 1;  // Reduce the alpha component by half!

        auto angle = 0.0;
        auto smallerCount = enemyShape.circle.getPointCount();
        for(auto i = 0; i < smallerCount; ++i) {
            Vec2f velocity = Vec2f::getCoOrdinates(enemyPlacement.velocity.x, angle);

            auto smallEnemy = entityManager.addEntity("smallEnemy");
            smallEnemy->template add<Placement_t>(enemyPlacement.pos, velocity, 0.0);
            smallEnemy->template add<Shape_t>(
                enemyShape.circle.getRadius()/2,
                enemyShape.circle.getPointCount(),
                fillColor,
                enemyShape.circle.getOutlineColor(),
                enemyShape.circle.getOutlineThickness()
            );
            smallEnemy->template add<Collision_t>(enemyCollision.radius / 2);
            smallEnemy->template add<Score_t>(enemyScore.score << 1);
            smallEnemy->template add<Lifespan_t>(std::min(enemyLifespan.lifespanInFrames >> 1, 300));

            angle += 360.0/smallerCount;
        }
    }
};

}   //namespace bv::lab::game::ecs
