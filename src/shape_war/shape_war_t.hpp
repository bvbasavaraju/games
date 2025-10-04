#pragma once

#include "config.hpp"
#include "entity_manager.hpp"
#include "player_t.hpp"
#include "bullet_t.hpp"
#include "enemy_t.hpp"
#include "special_weapon_t.hpp"

#include "movement_t.hpp"
#include "life_t.hpp"
#include "user_input_t.hpp"
#include "collide_t.hpp"
#include "display_t.hpp"

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
    sf::Font mFont;
    sf::Text mText;

    bool mPaused{false};
    bool mRunning{true};
    std::uint32_t mScore{0};
    std::uint32_t mCurrentFrame{0};
    std::uint32_t mLastEnemySpawnTime{0};
    std::uint32_t mScreenWidth{0};
    std::uint32_t mScreenHeight{0};
    std::uint32_t mSpecialWeaponCount {0};
    std::uint32_t mSpawningInterval{0};
    
    template <typename T>
    void debug() {
        static_assert(false, "check type now");
    }

    void init() {
        srand(time(0));
        
        mWindow.create(sf::VideoMode({mScreenWidth, mScreenHeight}), "ShapeWar");
        mWindow.setFramerateLimit(60);

        // TODO: add runtime config option using IMGUI: https://www.youtube.com/watch?v=-ugbLQlw_VM&list=PL_xRyXins84_Sq7yZkxGP_MgYAH-Zo8Uu&index=7
    }

    // Spawn entities:
    void enemySpanner() {
        if((mCurrentFrame - mLastEnemySpawnTime) > mSpawningInterval) {
            Enemy_t::create(mEntityManager, mScreenWidth, mScreenHeight);
            mLastEnemySpawnTime = mCurrentFrame;
        }
    }

    // Entity Movement
    void movement() {
        Movement_t::calculate(mEntityManager);
    }

    // Entity Life
    void lifespan() {
        Life_t::calculate(mEntityManager);
    }

    // Entity Collision
    void collision() {
        Collide_t::check(mEntityManager, mScore, mSpecialWeaponCount, player(), mScreenWidth, mScreenHeight);
    }

    // Display Entities
    void render() {
        Display_t::render(mEntityManager, mWindow, mText, mScore);
    }

    // User inputs
    void userInput() {
        UserInput_t::handle(mEntityManager, mWindow, player(), mSpecialWeaponCount, mPaused, mRunning);
    }

    auto player() -> std::shared_ptr<Entity_t> {
        return mEntityManager.getEntities("player").front();
    }

public:
    ShapeWar_t(uint32_t screenWidth_, uint32_t screenHeight_)
    : mScreenWidth(screenWidth_), 
      mScreenHeight(screenHeight_),
      mFont("./font/UbuntuMono-BI.ttf"),
      mText(mFont),
      mLastEnemySpawnTime(0),
      mSpawningInterval(45) {
        init();
    }

    void run() {     
        Player_t::create(mEntityManager, mScreenWidth, mScreenHeight);

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
