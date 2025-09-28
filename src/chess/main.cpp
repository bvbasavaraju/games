#include <iostream>

#include <SFML/Graphics.hpp>

// IMGUI Interactive manual: https://pthom.github.io/imgui_manual_online/manual/imgui_manual.html

int main() {
    std::cout << "Lets Begin!" << std::endl;
    sf::RenderWindow window(sf::VideoMode({1000, 1000}), "Chess Board");
    window.setFramerateLimit(60);

    // auto backgroundColor = sf::Color::Black;
    while (window.isOpen()) {

        // Handle any events
        while(const auto event = window.pollEvent()) {
            if(event->is<sf::Event::Closed>()) {
                window.close();
            } else if(const auto* keyPressed = event->getIf<sf::Event::KeyPressed>()) {
                if(keyPressed->scancode == sf::Keyboard::Scancode::Escape) {
                    window.close();
                }
            }
        }

        // Strips == Texture + Rectangle Entity
        // sf::RenderTexture texture({100, 100});
        // sf::Sprite sprite(texture.getTexture());

        // texture.draw(sprite);
        // texture.display();
        
        // window.draw(sprite);
        

        window.clear(); // window.clear(backgroundColor);

        // Draw border for chess board
        auto borderColor = sf::Color(100, 100, 100);
        sf::RectangleShape lineH({840, 20});
        lineH.setFillColor(borderColor);
        sf::RectangleShape lineV({20, 840});
        lineV.setFillColor(borderColor);

        lineH.setPosition({80, 80});
        window.draw(lineH);
        lineV.setPosition({80, 80});
        window.draw(lineV);

        lineH.setPosition({80, 900});
        window.draw(lineH);
        lineV.setPosition({900, 80});
        window.draw(lineV);
    
        // Draw chess board
        sf::RectangleShape square({100, 100});
        square.setFillColor(sf::Color::White);

        auto x = 100;
        auto y = 100;    
        for(auto i = 0; i < 8; ++i) {
            auto posX = i & 0x01 ? 200 : 100;
            for(auto j = 0; j < 4; ++j) {
                square.setPosition({static_cast<float>(posX), static_cast<float>(y)});
                window.draw(square);
                posX += 200;
            }
            y += 100;
        }

        // Display Window contents
        window.display();

        // backgroundColor = backgroundColor == sf::Color::Black? sf::Color::Red : sf::Color::Black;
    }
    
    return 0;
}
