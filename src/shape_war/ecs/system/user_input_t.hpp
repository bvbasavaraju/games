#pragma once

#include "bullet_t.hpp"
#include "special_weapon_t.hpp"

namespace bv::lab::game::ecs {

using namespace entity;

namespace system {
struct UserInput_t {

    static auto keyEvent(auto &event, auto player, auto &pause, auto &running) {
        // KeyPress event
        auto handleKeyEvent = [&](auto keyAction, bool isPressed) {
            auto &pInput = player->template get<Input_t>();
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
                    pause = isPressed ? !pause : pause;
                    break;

                default: 
                    break;
            }
        };

        // Close event
        if(event->template is<sf::Event::Closed>()) {
            running = false;
        } else if(auto const* keyPressed = event->template getIf<sf::Event::KeyPressed>()) {
            handleKeyEvent(keyPressed, true);
        } else if(auto const* keyReleased = event->template getIf<sf::Event::KeyReleased>()) {
            handleKeyEvent(keyReleased, false);
        }
    }

    static auto mouseEvent(auto &event, auto &entityManager, auto player, auto &specialWeaponCount) {
        // Mouse event
        if(auto const* mouseClick = event->template getIf<sf::Event::MouseButtonPressed>()) {
            if(mouseClick->button == sf::Mouse::Button::Left) {
                Vec2f mouseClickVec = { mouseClick->position.x * 1.0f, mouseClick->position.y * 1.0f };
                Bullet_t::create(entityManager, player, mouseClickVec);
            }

            if(mouseClick->button == sf::Mouse::Button::Right) { 
                if(specialWeaponCount <= 0) {
                    return;
                }

                SpecialWeapon_t::create(entityManager, player);
                specialWeaponCount--;
            }
        }
    }

public:
    // Player Creation
    static auto handle(auto &entityManager, auto &window, auto player, auto &specialWeaponCount, auto &pause, auto &running) {
        // TODO: IMGUI process
        while(auto event = window.pollEvent()) {
            keyEvent(event, player, pause, running);
            mouseEvent(event, entityManager, player, specialWeaponCount);
        }
    }
};

}   // namespace system
}   // bv::lab::game::ecs
