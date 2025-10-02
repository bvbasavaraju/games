#pragma once

namespace bv::lab::game::ecs {

using namespace entity;

namespace system {
struct Life_t {
    // Player Creation
    static auto calculate(auto &entityManager) {
        for(auto &e : entityManager.getEntities()) {
            if(!e->template has<Lifespan_t>()) {
                continue;
            }

            auto &entityLifespan = e->template get<Lifespan_t>();
            entityLifespan.remainingLife -= 1;
            if(!entityLifespan.remainingLife) {
                e->destroy();
            }
        }
    }
};

}   // namespace system
}   // bv::lab::game::ecs
