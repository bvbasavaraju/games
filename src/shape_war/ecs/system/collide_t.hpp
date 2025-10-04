#pragma once

namespace bv::lab::game::ecs {

using namespace entity;

namespace system {
struct Collide_t {
private:
    // Get radius!
    constexpr static auto getRadiusSum = [](auto entity1, auto entity2, bool useCollisionRadius) {
        auto e1Radius = entity1->template get<Collision_t>().radius;
        auto e2Radius = entity2->template get<Collision_t>().radius;

        if(!useCollisionRadius) {
            e1Radius = entity1->template get<Shape_t>().circle.getRadius();
            e2Radius = entity2->template get<Shape_t>().circle.getRadius();
        }

        return e1Radius + e2Radius;
    };

    // Entity Collision check
    constexpr static auto entityCollisionCheck = [](auto entity1, auto entity2, bool useCollisionRadius = true) -> bool {
        auto &e1Pos = entity1->template get<Placement_t>().pos;
        auto &e2Pos = entity2->template get<Placement_t>().pos;

        auto distVec = e1Pos - e2Pos;
        auto distSquare = (distVec.x * distVec.x) + (distVec.y * distVec.y);

        auto radiusSum = getRadiusSum(entity1, entity2, useCollisionRadius);
        if(distSquare <= (radiusSum * radiusSum)) {
            return true;
        }

        return false;
    };

    // Update the score
    constexpr static auto updateScore = [](auto &score, auto entity, auto &specialWeaponCount, bool isSpecialBullet = false) {
        auto score_ = entity->template get<Score_t>().score;
        static auto prevScore = score_;
        
        score += score_;
        if(isSpecialBullet) {
            score += score_;
            prevScore = score;
        }

        // Increase a special weapon count if the score is added with 500 points
        if(!isSpecialBullet && ((score - prevScore) >= 500)) {
            specialWeaponCount++;
            prevScore = score;
        }
    };

    // Wall Hit Action
    constexpr static auto checkAndActOnWallHit = [](auto entity, auto screenWidth, auto screenHeight, bool changeDirection) {
        auto esr = entity->template get<Shape_t>().circle.getRadius();
        auto &ePlacement = entity->template get<Placement_t>();
        auto &input = entity->template get<Input_t>();

        // RightWall
        if(ePlacement.pos.x + esr >= screenWidth) {
            if(changeDirection) {
                ePlacement.velocity.x *= -1.0;
            } else {
                input.right = false;
            }
        }
        // LeftWall
        if(ePlacement.pos.x - esr <= 0) {
            if(changeDirection) {
                ePlacement.velocity.x *= -1.0;
            } else {
                input.left = false;
            }
        }
        // Floor
        if(ePlacement.pos.y + esr >= screenHeight) {
            if(changeDirection) {
                ePlacement.velocity.y *= -1.0;
            } else {
                input.down = false;
            }
        }
        // Ceil
        if(ePlacement.pos.y - esr <= 0) {
            if(changeDirection) {
                ePlacement.velocity.y *= -1.0;
            } else {
                input.up = false;
            }
        }
    };

    // Weapon hitting enemies
    constexpr static auto weaponHittingEnemies = [](auto &entityManager, auto &weapons, auto &enemies, auto &score, auto &specialWeaponCount, bool isSmaller = false, bool isSpecial = false) -> void {
        for(auto &weapon: weapons) {
            for(auto &enemy: enemies) {
                if(entityCollisionCheck(weapon, enemy)) {
                    updateScore(score, enemy, specialWeaponCount, isSpecial);
                    enemy->destroy();

                    if(!isSmaller) {
                        Enemy_t::createSmaller(entityManager, enemy);
                    }

                    if(!isSpecial) {
                        weapon->destroy();
                        break;
                    }
                }
            }
        }
    };

    // Non Player hitting wall
    constexpr static auto entityHittingWall = [](auto entities, auto screenWidth, auto screenHeight) -> void {
        for(auto entity: entities) {
            checkAndActOnWallHit(entity, screenWidth, screenHeight, true);
        }
    };

    // Player collision check
    constexpr static auto playerCollidingWithEnemy = [](auto &entityManager, auto player, auto &enemies, bool isSmaller = false) -> void {
        for(auto &enemy: enemies) {
            if(entityCollisionCheck(player, enemy, false)) {
                enemy->destroy();
                if(!isSmaller) {
                    Enemy_t::createSmaller(entityManager, enemy);
                }
            }
        }
    };

public:
    // Player Creation
    static auto check(
        auto &entityManager, 
        auto &score, 
        auto &specialWeaponCount, 
        auto player,
        auto screenWidth, 
        auto screenHeight) {

        auto &bullets = entityManager.getEntities("bullet");
        auto &specialBullet = entityManager.getEntities("specialBullet");
        auto &enemies = entityManager.getEntities("enemy");
        auto &smallEnemies = entityManager.getEntities("smallEnemy");        


        // Bullet hitting enemy!!
        weaponHittingEnemies(entityManager, bullets, enemies, score, specialWeaponCount);
        weaponHittingEnemies(entityManager, bullets, smallEnemies, score, specialWeaponCount, true);

        // Special Bullet hit!!
        weaponHittingEnemies(entityManager, specialBullet, enemies, score, specialWeaponCount, false, true);
        weaponHittingEnemies(entityManager, specialBullet, smallEnemies, score, specialWeaponCount, true, true);

        // entities hitting wall!!
        entityHittingWall(enemies, screenWidth, screenHeight);
        entityHittingWall(smallEnemies, screenWidth, screenHeight);

        // Player collision with wall!
        checkAndActOnWallHit(player, screenWidth, screenHeight, false);

        // Enemy and player collision!!
        playerCollidingWithEnemy(entityManager, player, enemies);
        playerCollidingWithEnemy(entityManager, player, smallEnemies, true);
    }
};

}   // namespace system
}   // bv::lab::game::ecs
