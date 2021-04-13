#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>

using std::vector;
int main() {

    sf::RenderWindow window(sf::VideoMode(1024, 768), "", sf::Style::Close);
    window.setFramerateLimit(60);
    sf::RectangleShape rect(sf::Vector2f(1024, 768));

    sf::Shader shader;
    if (!shader.loadFromFile("shaders/voronoy.frag", sf::Shader::Fragment)) {
        std::cout << "no shader file!";
        return 1;
    }
    shader.setUniform("iResolution", sf::Vector2<float>(1024, 768));


    auto start = clock();

    while (window.isOpen()) {
        shader.setUniform("iTime", float(clock() - start) / CLOCKS_PER_SEC);
        sf::Event event;
        while (window.pollEvent(event))
            if (event.type == sf::Event::Closed)
                window.close();
        window.clear();
        window.draw(rect, &shader);
        window.display();
    }
}