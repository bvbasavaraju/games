#pragma once

#include "entity.hpp"

// std includes
#include <vector>
#include <memory>
#include <map>

namespace bv::lab::game::ecs::entity {

using EntityVector_t = std::vector<std::shared_ptr<Entity_t>>;
using EntityMap_t = std::unordered_map<std::string, EntityVector_t>;

class EntityManager_t {
private:
    EntityVector_t mEntities;
    EntityVector_t mToAdd;
    EntityMap_t mEntitiesInGroup;

    auto removeDeadEntities() -> void {
        auto removeIfNotActive = [&](EntityVector_t& entities) -> void {
            std::erase_if(entities, [&](auto e) -> bool {
                return !e->isActive();
            });
        };
        

        removeIfNotActive(mEntities);
        for(auto& [tag, entities] : mEntitiesInGroup) {
            removeIfNotActive(entities);
        }
    };

public:
    EntityManager_t() = default;

    auto update() -> void {
        removeDeadEntities();

        for(auto e : mToAdd) {
            mEntities.push_back(e);
            mEntitiesInGroup[e->tag()].push_back(e);
        }

        mToAdd.clear();
    }

    auto addEntity(std::string const& tag) -> std::shared_ptr<Entity_t> {
        auto entity = std::make_shared<Entity_t>(Entity_t(mEntities.size()+1, tag));

        mToAdd.push_back(entity);
        return entity;
    }

    auto getEntities() -> EntityVector_t const& {
        return mEntities;
    }

    auto getEntities(std::string const& tag) -> EntityVector_t const& {
        return mEntitiesInGroup[tag];
    }

    auto getEntityMap() -> EntityMap_t const& {
        return mEntitiesInGroup;
    }
};  

}   // namespace bv::lab::game::ecs::entity
