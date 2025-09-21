#include <SFML/Graphics.hpp>
#include <iostream>
int main() {
    std::cout<<"LES GO";
    sf::RenderWindow window(sf::VideoMode(800, 600), "Hello SFML + Dev-C++");
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear(sf::Color::Black);
        window.display();
    }
    std::cout<<"DONE";
    return 0;
}
