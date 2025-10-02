#pragma once

namespace bv::lab::game::ecs {

using namespace entity;

namespace system {
struct Movement_t {
    // Player Creation
    static auto calculate(auto &entityManager) {
        auto handleInputEntity = [](auto &entity) {
            auto &placement = entity->template get<Placement_t>();
            auto &input = entity->template get<Input_t>();
            if(input.up) {
                placement.pos.y -= placement.velocity.y;
            } else if (input.down) {
                placement.pos.y += placement.velocity.y;
            } else if (input.left) {
                placement.pos.x -= placement.velocity.x;
            } else if (input.right) {
                placement.pos.x += placement.velocity.x;
            } else {
                // placement.pos += placement.velocity;
            }
        };

        for(auto &entity : entityManager.getEntities()) {
            if(entity->template has<Input_t>()) {
                handleInputEntity(entity);
            } else {
                auto &entityPlacement = entity->template get<Placement_t>();
                entityPlacement.pos += entityPlacement.velocity;
            }
        }
    }
};

}   // namespace system
}   // bv::lab::game::ecs
