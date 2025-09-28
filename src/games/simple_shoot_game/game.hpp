#pragma once

#include "ecs/config.hpp"
#include "ecs/entity_manager.hpp"

// STL includes
#include <cstdlib>
#include <ctime>

namespace bv::lab {

class Game_t {
private:
    
    PlayerConfig_t mPlayerConfig;
    EnemyConfig_t mEnemyConfig;
    BulletConfig_t mBulletConfig;
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

    void setPlayerConfig() {
        mPlayerConfig.shapeRadius = 32;
        mPlayerConfig.collisionRadius = 32;
        mPlayerConfig.outlineThickness = 4;
        mPlayerConfig.fillColor.red = 5;
        mPlayerConfig.fillColor.green = 5;
        mPlayerConfig.fillColor.blue = 5;
        mPlayerConfig.outlineColor.red = 255;
        mPlayerConfig.outlineColor.green = 0;
        mPlayerConfig.outlineColor.blue = 0;
        
        mPlayerConfig.speed = 5;
        mPlayerConfig.shapeVertices = 8;
    };

    void setEnemyConfig() {
        mEnemyConfig.shapeRadius = 32;
        mEnemyConfig.collisionRadius = 32;
        mEnemyConfig.outlineThickness = 2;
        mEnemyConfig.fillColor.red = 128;
        mEnemyConfig.fillColor.green = 128;
        mEnemyConfig.fillColor.blue = 128;
        mEnemyConfig.outlineColor.red = 255;
        mEnemyConfig.outlineColor.green = 255;
        mEnemyConfig.outlineColor.blue = 255;

        mEnemyConfig.speedMin = 1;
        mEnemyConfig.speedMax = 1;
        mEnemyConfig.verticesMin = 3;
        mEnemyConfig.verticesMax = 8;
        mEnemyConfig.lifespan = 1200;
        mEnemyConfig.spawnInterval = 90;
    };

    void setBulletConfig() {
        mBulletConfig.shapeRadius = 10;
        mBulletConfig.collisionRadius = 10;
        mBulletConfig.outlineThickness = 2;
        mBulletConfig.fillColor.red = 255;
        mBulletConfig.fillColor.green = 255;
        mBulletConfig.fillColor.blue = 255;
        mBulletConfig.outlineColor.red = 255;
        mBulletConfig.outlineColor.green = 255;
        mBulletConfig.outlineColor.blue = 255;
        
        mBulletConfig.speed = 20;
        mBulletConfig.shapeVertices = 20;
        mBulletConfig.lifespan = 90;
    };

    void init() {
        srand(time(0));

        setPlayerConfig();
        setEnemyConfig();
        setBulletConfig();

        mWindow.create(sf::VideoMode({mScreenWidth, mScreenHeight}), "ShapeWar");
        mWindow.setFramerateLimit(60);

        // TODO: add runtime config option using IMGUI: https://www.youtube.com/watch?v=-ugbLQlw_VM&list=PL_xRyXins84_Sq7yZkxGP_MgYAH-Zo8Uu&index=7

        spawnPlayer();
    }

    void spawnPlayer() {
        auto player = mEntityManager.addEntity("player");
        player->add<Placement_t>(
            Vec2f{static_cast<float>(mScreenWidth >> 1), static_cast<float>(mScreenHeight >> 1)}, 
            Vec2f{1.0f, 1.0f}, 
            0.0);
        
        player->add<Shape_t>(
            mPlayerConfig.shapeRadius, 
            mPlayerConfig.shapeVertices, 
            sf::Color{mPlayerConfig.fillColor.red, mPlayerConfig.fillColor.green, mPlayerConfig.fillColor.blue},
            sf::Color{mPlayerConfig.outlineColor.red, mPlayerConfig.outlineColor.green, mPlayerConfig.outlineColor.blue},
            mPlayerConfig.outlineThickness);

        player->add<Input_t>();
    }

    void spawnEnemies() {
        auto enemy = mEntityManager.addEntity("enemy");

        // adjust the position if its in the boundary!
        auto adjustIfInBoundary = [&](auto &pos, auto end) {
            if((pos < (end << 1) && pos < (mEnemyConfig.shapeRadius * 2))) {
                pos = mEnemyConfig.shapeRadius * 2;
            } else if(pos > (end << 1) && pos > (end - mEnemyConfig.shapeRadius * 2)) {
                pos = end - mEnemyConfig.shapeRadius * 2;
            }
        };

        // Position calc
        auto posX = (rand() %  mScreenWidth);
        auto posY = (rand() %  mScreenHeight);

        adjustIfInBoundary(posX, mScreenWidth);
        adjustIfInBoundary(posY, mScreenHeight);

        // Velocity Calc
        auto velX = (posX > (mScreenWidth >> 1)) ? mEnemyConfig.speedMin * -1.0f : mEnemyConfig.speedMin;
        auto velY = (posY > (mScreenHeight >> 1)) ? mEnemyConfig.speedMin * -1.0f : mEnemyConfig.speedMin;

        auto edges = rand() % (mEnemyConfig.verticesMax - mEnemyConfig.verticesMin + 1);
        auto vertices = edges + mEnemyConfig.verticesMin;   // Offset of 3 vertices

        auto fillColor = fillColors[edges % fillColors.size()];
        fillColor.a = 150;

        // add necessary components
        enemy->add<Placement_t>(Vec2f{posX * 1.0f, posY * 1.0f}, Vec2f{velX, velY}, 0.0);
        enemy->add<Shape_t>(
            mEnemyConfig.shapeRadius,
            vertices,
            fillColor,
            sf::Color{mEnemyConfig.outlineColor.red, mEnemyConfig.outlineColor.green, mEnemyConfig.outlineColor.blue},
            mEnemyConfig.outlineThickness
        );
        enemy->add<Collision_t>(mEnemyConfig.collisionRadius);
        enemy->add<Score_t>((vertices - mEnemyConfig.verticesMin + 1) * 10);
        enemy->add<Lifespan_t>(mEnemyConfig.lifespan);

        // updated current frame so that auto spawn is possible
        mLastEnemySpawnTime = mCurrentFrame;
    }

    // This is used when bullet hits the enemy
    void spawnEnemyDestruction(auto &entity) {
        auto &enemyPlacement = entity->template get<Placement_t>();
        auto &enemyShape = entity->template get<Shape_t>();
        auto &enemyCollision = entity->template get<Collision_t>();
        auto &enemyScore = entity->template get<Score_t>();
        auto &enemyLifespan = entity->template get<Lifespan_t>();

        auto fillColor = enemyShape.circle.getFillColor();
        fillColor.a >>= 1;  // Reduce the alpha component by half!

        auto angle = 0.0;
        auto smallerCount = enemyShape.circle.getPointCount();
        for(auto i = 0; i < smallerCount; ++i) {
            Vec2f velocity = Vec2f::getCoOrdinates(mEnemyConfig.speedMin, angle);

            auto smallEnemy = mEntityManager.addEntity("smallEnemy");
            smallEnemy->add<Placement_t>(enemyPlacement.pos, velocity, 0.0);
            smallEnemy->add<Shape_t>(
                enemyShape.circle.getRadius()/2,
                enemyShape.circle.getPointCount(),
                fillColor,
                enemyShape.circle.getOutlineColor(),
                enemyShape.circle.getOutlineThickness()
            );
            smallEnemy->add<Collision_t>(enemyCollision.radius / 2);
            smallEnemy->add<Score_t>(enemyScore.score << 1);
            smallEnemy->add<Lifespan_t>(std::min(enemyLifespan.lifespanInFrames >> 1, 300));

            angle += 360.0/smallerCount;
        }
    }

    void spawnSpecialWeapon() {
        if(mSpecialWeaponCount <= 0) {
            return;
        }

        auto player_ = player();
        auto &playerPlacement = player_->template get<Placement_t>();
        auto &playerShape = player_->template get<Shape_t>();

        auto angle = 0.0;
        auto bulletCount = playerShape.circle.getPointCount();
        for(auto i = 0; i < bulletCount; ++i) {
            Vec2f velocity = Vec2f::getCoOrdinates(mBulletConfig.speed, angle);

            auto special = mEntityManager.addEntity("specialBullet");
            special->add<Placement_t>(playerPlacement.pos, velocity, 0.0);
            special->add<Shape_t>(
                mBulletConfig.shapeRadius,
                mBulletConfig.shapeVertices,
                sf::Color::Red,
                sf::Color::Red,
                mBulletConfig.outlineThickness
            );
            special->add<Collision_t>(mBulletConfig.collisionRadius);
            special->add<Lifespan_t>(mBulletConfig.lifespan);

            angle += 360.0/bulletCount;
        }

        mSpecialWeaponCount--;
    }

    // Spawn bullet from player's location to target location (mouse click)
    void spawnBullets(auto playerEntity, Vec2f const& mousePosVec) {
        auto &playerPosVec = playerEntity->template get<Placement_t>().pos;
        
        // Distance vector
        auto distVec = mousePosVec - playerPosVec;
        distVec.normalize();

        // Bullet speed and velocity
        auto bulletVelocity = Vec2f{distVec.x * mBulletConfig.speed, distVec.y * mBulletConfig.speed};
        float bulletAngle = distVec.angle();

        // Create Bullet entity
        auto bullet = mEntityManager.addEntity("bullet");
        bullet->add<Placement_t>(playerPosVec, bulletVelocity, bulletAngle);
        bullet->add<Shape_t>(
            mBulletConfig.shapeRadius,
            mBulletConfig.shapeVertices,
            sf::Color{mBulletConfig.fillColor.red, mBulletConfig.fillColor.green, mBulletConfig.fillColor.blue},
            sf::Color{mBulletConfig.outlineColor.red, mBulletConfig.outlineColor.green, mBulletConfig.outlineColor.blue},
            mBulletConfig.outlineThickness);
        bullet->add<Lifespan_t>(mBulletConfig.lifespan);
        bullet->add<Collision_t>(mBulletConfig.collisionRadius);
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
        if((mCurrentFrame - mLastEnemySpawnTime) > mEnemyConfig.spawnInterval) {
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
                    spawnBullets(player(), {static_cast<float>(mouseClick->position.x), static_cast<float>(mouseClick->position.y)});
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
    Game_t(uint32_t screenWidth_, uint32_t screenHeight_)
    : mScreenWidth(screenWidth_), 
      mScreenHeight(screenHeight_),
      font("../../font/UbuntuMono-BI.ttf"),
      text(font) {
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
