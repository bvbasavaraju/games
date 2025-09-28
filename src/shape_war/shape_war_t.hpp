#pragma once

#include "config.hpp"
#include "entity_manager.hpp"
#include "spawner_t.hpp"

// STL includes
#include <cstdlib>
#include <ctime>

namespace bv::lab::game {

using namespace ecs::entity;
using namespace ecs::component;
using namespace ecs::system;

class ShapeWar_t {
private:
    EntityManager_t mEntityManager;
    
    sf::RenderWindow mWindow;
    sf::Clock mDeltaClock;
    sf::Font font;
    sf::Text text;

    bool mPaused{false};
    bool mRunning{true};
    std::uint32_t mScore;
    std::uint32_t mCurrentFrame;
    std::uint32_t mLastEnemySpawnTime;
    std::uint32_t mScreenWidth;
    std::uint32_t mScreenHeight;
    std::uint32_t mSpecialWeaponCount;
    std::uint32_t mSpawningInterval;

    std::array<sf::Color, 6> fillColors = {
        sf::Color::Red, 
        sf::Color::Green,
        sf::Color::Blue,
        sf::Color::Yellow,
        sf::Color::Magenta,
        sf::Color::Cyan };
    
    template <typename T>
    void debug() {
        static_assert(false, "check type now");
    }

    void init() {
        srand(time(0));
        
        mWindow.create(sf::VideoMode({mScreenWidth, mScreenHeight}), "ShapeWar");
        mWindow.setFramerateLimit(60);

        // TODO: add runtime config option using IMGUI: https://www.youtube.com/watch?v=-ugbLQlw_VM&list=PL_xRyXins84_Sq7yZkxGP_MgYAH-Zo8Uu&index=7

        Spawner_t::createPlayer(mEntityManager, mScreenWidth, mScreenHeight);
    }

    void spawnEnemies() {
        Spawner_t::createEnemy(mEntityManager, mScreenWidth, mScreenHeight);
        mLastEnemySpawnTime = mCurrentFrame;
    }

    // This is used when bullet hits the enemy
    void spawnEnemyDestruction(auto &entity) {
        Spawner_t::createSmallerEnemy(mEntityManager, entity);
    }

    void spawnSpecialWeapon() {
        if(mSpecialWeaponCount <= 0) {
            return;
        }

        Spawner_t::createSpecialWeapon(mEntityManager, player());

        mSpecialWeaponCount--;
    }

    void movement() {

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

        for(auto &entity : mEntityManager.getEntities()) {
            if(entity->has<Input_t>()) {
                handleInputEntity(entity);
            } else {
                auto &entityPlacement = entity->get<Placement_t>();
                entityPlacement.pos += entityPlacement.velocity;
            }
        }
    }

    void lifespan() {
        for(auto &e : mEntityManager.getEntities()) {
            if(!e->has<Lifespan_t>()) {
                continue;
            }

            auto &entityLifespan = e->get<Lifespan_t>();
            entityLifespan.remainingLife -= 1;
            if(!entityLifespan.remainingLife) {
                e->destroy();
            }
        }
    }

    void collision() {
        auto &bullets = mEntityManager.getEntities("bullet");
        auto &specialBullet = mEntityManager.getEntities("specialBullet");
        auto &enemies = mEntityManager.getEntities("enemy");
        auto &smallEnemies = mEntityManager.getEntities("smallEnemy");        

        // destroy bullet and enemy!!
        auto hasCollided = [&](auto entity1, auto entity2, bool useCollisionRadius = true) -> bool {
            auto getRadiusSum = [&]() {
                auto e1Radius = entity1->template get<Collision_t>().radius;
                auto e2Radius = entity2->template get<Collision_t>().radius;

                if(!useCollisionRadius) {
                    e1Radius = entity1->template get<Shape_t>().circle.getRadius();
                    e2Radius = entity2->template get<Shape_t>().circle.getRadius();
                }

                return e1Radius + e2Radius;
            };

            auto &e1Pos = entity1->template get<Placement_t>().pos;
            auto &e2Pos = entity2->template get<Placement_t>().pos;

            auto distVec = e1Pos - e2Pos;
            auto distSquare = (distVec.x * distVec.x) + (distVec.y * distVec.y);

            auto radiusSum = getRadiusSum();
            if(distSquare <= (radiusSum * radiusSum)) {
                return true;
            }

            return false;
        };

        // Score keeper!
        auto updateScore = [&](auto score, bool isSpecialBullet = false) {
            static auto prevScore = mScore;
            
            mScore += score;
            if(isSpecialBullet) {
                mScore += score;
                prevScore = mScore;
            }

            // Increase a special weapon count if the score is added with 500 points
            if((mScore - prevScore) > 500) {
                mSpecialWeaponCount++;
                prevScore = mScore;
            }
        };

        // Bullet hitting enemy!!
        for(auto &bullet: bullets) {
            for(auto &enemy: enemies) {
                if(hasCollided(bullet, enemy)) {
                    enemy->destroy();
                    bullet->destroy();
                    spawnEnemyDestruction(enemy);
                    updateScore(enemy->template get<Score_t>().score);
                    break;
                }
            }

            for(auto &smallEnemy: smallEnemies) {
                if(hasCollided(bullet, smallEnemy)) {
                    smallEnemy->destroy();
                    bullet->destroy();
                    updateScore(smallEnemy->template get<Score_t>().score);
                    break;
                }
            }
        }

        // Special Bullet hit!!
        for(auto &specialBullet_: specialBullet) {
            for(auto &enemy: enemies) {
                if(hasCollided(specialBullet_, enemy)) {
                    enemy->destroy();
                    spawnEnemyDestruction(enemy);
                    updateScore(enemy->template get<Score_t>().score, true);
                }
            }

            for(auto &smallEnemy: smallEnemies) {
                if(hasCollided(specialBullet_, smallEnemy)) {
                    smallEnemy->destroy();
                    updateScore(smallEnemy->template get<Score_t>().score, true);
                }
            }
        }

        // entities hitting wall!!
        auto changeDirectionIfHitsWall = [&](auto entity) {
            auto esr = entity->template get<Shape_t>().circle.getRadius();
            auto &ePlacement = entity->template get<Placement_t>();

            // RightWall
            if(ePlacement.pos.x + esr >= mScreenWidth) {
                ePlacement.velocity.x *= -1.0;
            }
            // LeftWall
            if(ePlacement.pos.x - esr <= 0) {
                ePlacement.velocity.x *= -1.0;
            }
            // Floor
            if(ePlacement.pos.y + esr >= mScreenHeight) {
                ePlacement.velocity.y *= -1.0;
            }
            // Ceil
            if(ePlacement.pos.y - esr <= 0) {
                ePlacement.velocity.y *= -1.0;
            }
        };

        for(auto &enemy : enemies) {
            changeDirectionIfHitsWall(enemy);
        }

        for(auto &smallEnemy : smallEnemies) {
            changeDirectionIfHitsWall(smallEnemy);
        }

        // Check player collision with wall!
        auto stopPlayerMovementIfHitsWall = [&]() {
            auto player_ = player();
            auto psr = player_->template get<Shape_t>().circle.getRadius();
            auto &pPlacement = player_->template get<Placement_t>();
            auto &pInput = player_->template get<Input_t>();

            // RightWall
            if(pPlacement.pos.x + psr >= mScreenWidth) {
                pInput.right = false;
            }
            // LeftWall
            if(pPlacement.pos.x - psr <= 0) {
                pInput.left = false;
            }
            // Floor
            if(pPlacement.pos.y + psr >= mScreenHeight) {
                pInput.down = false;
            }
            // Ceil
            if(pPlacement.pos.y - psr <= 0) {
                pInput.up = false;
            }
        };

        stopPlayerMovementIfHitsWall();

        // Enemy and player collision!!
        for(auto &enemy: enemies) {
            if(hasCollided(player(), enemy, false)) {
                enemy->destroy();
                spawnEnemyDestruction(enemy);
            }
        }

        for(auto &smallEnemy: smallEnemies) {
            if(hasCollided(player(), smallEnemy, false)) {
                smallEnemy->destroy();
            }
        }
    }

    void enemySpanner() {
        if((mCurrentFrame - mLastEnemySpawnTime) > mSpawningInterval) {
            spawnEnemies();
        }
    }

    void render() {
        static std::string scoreCardStr("ScoreCard: ");
        
        mWindow.clear();

        // render entities
        auto render_ = [&](auto entity) {
            auto &shape = entity->template get<Shape_t>();
            auto &placement = entity->template get<Placement_t>();

            placement.angle += 1.0;
            shape.circle.setPosition(placement.pos);
            shape.circle.setRotation(sf::degrees(placement.angle));

            mWindow.draw(shape.circle);
        };   

        // Render every entity!!
        for(auto &e : mEntityManager.getEntities()) {
            render_(e);
        }

        // Draw scorecard!!
        text.setString(scoreCardStr + std::to_string(mScore));
        mWindow.draw(text); 
        
        mWindow.display();
    }

    void userInput() {
        // TODO: IMGUI process
        while(auto event = mWindow.pollEvent()) {
            // KeyPress event
            auto handleKeyEvent = [&](auto keyAction, bool isPressed) {
                auto &pInput = player()->get<Input_t>();
                switch(keyAction->scancode) {
                    case sf::Keyboard::Scancode::W:
                        pInput.up = isPressed;
                        break;

                    case sf::Keyboard::Scancode::S:
                        pInput.down = isPressed;
                        break;

                    case sf::Keyboard::Scancode::A:
                        pInput.left = isPressed;
                        break;

                    case sf::Keyboard::Scancode::D:
                        pInput.right = isPressed;
                        break;

                    case sf::Keyboard::Scancode::P:
                        mPaused = isPressed ? !mPaused : mPaused;
                        break;

                    default: 
                        break;
                }
            };

            // Close event
            if(event->is<sf::Event::Closed>()) {
                mRunning = false;
            } else if(auto const* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                handleKeyEvent(keyPressed, true);
            } else if(auto const* keyReleased = event->getIf<sf::Event::KeyReleased>()) {
                handleKeyEvent(keyReleased, false);
            }

            // Mouse event
            if(auto const* mouseClick = event->getIf<sf::Event::MouseButtonPressed>()) {
                if(mouseClick->button == sf::Mouse::Button::Left) {
                    Vec2f mouseClickVec = { mouseClick->position.x * 1.0f, mouseClick->position.y * 1.0f };
                    Spawner_t::createBullet(mEntityManager, player(), mouseClickVec);
                }

                if(mouseClick->button == sf::Mouse::Button::Right) { 
                    spawnSpecialWeapon();
                }
            }
        }
    }

    auto player() -> std::shared_ptr<Entity_t> {
        return mEntityManager.getEntities("player").front();
    }

public:
    ShapeWar_t(uint32_t screenWidth_, uint32_t screenHeight_)
    : mScreenWidth(screenWidth_), 
      mScreenHeight(screenHeight_),
      font("./font/UbuntuMono-BI.ttf"),
      text(font),
      mLastEnemySpawnTime(0),
      mSpawningInterval(90) {
        init();
    }

    void run() {        
        while(mRunning) {
            mEntityManager.update();

            if(!mPaused) {
                enemySpanner();
                collision();
                movement();
                lifespan();
            }

            userInput();
            render();

            mCurrentFrame++;
        }
    }
};  
}   // namespace bv::lab
