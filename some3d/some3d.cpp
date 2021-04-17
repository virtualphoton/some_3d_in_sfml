#include <SFML/Graphics.hpp>
#include <iostream>
#include <ctime>
#include "camera.h"
#include "vectors.h"

const int W = 1024;
const int H = 768;

using std::vector;
int main() {

    sf::RenderWindow window(sf::VideoMode(W, H), "", sf::Style::Close);
    window.setFramerateLimit(60);
    sf::RectangleShape rect(sf::Vector2f(W, H));
    sf::Shader shader;
    if (!shader.loadFromFile("shaders/simple3d.frag", sf::Shader::Fragment)) {
        std::cout << "no shader file!";
        return 1;
    }
    shader.setUniform("iResolution", sf::Vector2<float>(W, H));
    vec3 c_pos(-3, 1, 2);
    Camera cam(c_pos, {0, 0, 1}, {0, 1, 0}, true);
    auto start = clock();
    sf::Vector2i center(W / 2, H / 2);
    bool space_pressed = false;
    while (window.isOpen()) {
        shader.setUniform("iTime", float(clock() - start) / CLOCKS_PER_SEC);
        cam.send_uniforms(shader);
        sf::Event event;
        while (window.pollEvent(event))
            if (event.type == sf::Event::Closed or sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)){
                window.close();
                break;
            }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
            cam.rotate_angles(0, -.05, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
            cam.rotate_angles(0, .05, 0);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LShift))
            cam.orig += cam.up*.02;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::LControl))
            cam.orig -= cam.up * .02;

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            cam.orig += cam.forward*.02;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            cam.orig -= cam.forward*.02;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            cam.orig += cam.right * .02;
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            cam.orig -= cam.right * .02;


        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
            if (space_pressed) {
                auto cur_shift = sf::Mouse::getPosition(window) - center;
                cam.rotate_px(cur_shift.x, -cur_shift.y);
            } else
                space_pressed = true;
            sf::Mouse::setPosition(center, window);

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