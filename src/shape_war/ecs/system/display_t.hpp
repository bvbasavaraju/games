#pragma once

namespace bv::lab::game::ecs {

using namespace entity;

namespace system {
struct Display_t {
    // Player Creation
    static auto render(auto &entityManager, auto &window, auto &text, auto score) {
        static std::string scoreCardStr("ScoreCard: ");
        
        window.clear();

        // render entities
        auto render_ = [&](auto entity) {
            auto &shape = entity->template get<Shape_t>();
            auto &placement = entity->template get<Placement_t>();

            placement.angle += 1.0;
            shape.circle.setPosition(placement.pos);
            shape.circle.setRotation(sf::degrees(placement.angle));

            window.draw(shape.circle);
        };   

        // Render every entity!!
        for(auto &e : entityManager.getEntities()) {
            render_(e);
        }

        // Draw scorecard!!
        text.setString(scoreCardStr + std::to_string(score));
        window.draw(text); 
        
        window.display();
    }
};

}   // namespace system
}   // bv::lab::game::ecs
