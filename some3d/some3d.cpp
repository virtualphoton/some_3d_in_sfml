#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include "camera.h"
#include "vectors.h"

#define W 1024
#define H 768

using std::vector;
int main() {

    sf::RenderWindow window(sf::VideoMode(W, H), "", sf::Style::Close);
    window.setFramerateLimit(60);
    sf::RectangleShape rect(sf::Vector2f(W, H));
    sf::Shader shader;
    if (!shader.loadFromFile("shaders/camera_test.frag", sf::Shader::Fragment)) {
        std::cout << "no shader file!";
        return 1;
    }
    shader.setUniform("iResolution", sf::Vector2<float>(W, H));
    vec3 c_pos(4, 0.5, 0);
    Camera cam(c_pos, vec3(.5) - c_pos);

    auto start = clock();
    sf::Vector2i center(W / 2, H / 2);
    bool space_pressed = false;
    while (window.isOpen()) {
        shader.setUniform("iTime", float(clock() - start) / CLOCKS_PER_SEC);
        cam.set_uniforms(shader);
        sf::Event event;
        while (window.pollEvent(event))
            switch (event.type){
                case sf::Event::Closed:
                    window.close();
                    break;
            }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            if (space_pressed) {
                auto cur_shift = sf::Mouse().getPosition(window) - center;
                cam.rotate_px(cur_shift.x, -cur_shift.y);
            } else
                space_pressed = true;
            sf::Mouse().setPosition(center, window);

            window.setMouseCursorVisible(false);
        } else {
            space_pressed = false;
            window.setMouseCursorVisible(true);
        }

        window.clear();

        window.draw(rect, &shader);
        window.display();
    }
}