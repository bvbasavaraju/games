#include <iostream>

// #include <vector2.hpp>
#include "game.hpp"

int main() {
    // bv::lab::Vec2f p(10, 10);
    // bv::lab::Vec2f e1(5, 5);

    // auto added = p+e1;

    // std::cout << p.getX() << " " << p.getY() << std::endl;
    // std::cout << added.getX() << " " << added.getY() << std::endl;

    // p += e1;
    // std::cout << p.getX() << " " << p.getY() << std::endl;

    bv::lab::Game_t shapeWar(1280, 720);
    shapeWar.run();

    return 0;
}
