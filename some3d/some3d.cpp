#include <SFML/Graphics.hpp>
#include <iostream>
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
        std::cout << "no shader file or shader error";
        return 1;
    }
    shader.setUniform("iResolution", sf::Vector2<float>(W, H));

    vec3 c_pos(-2, 6, 20);
    Camera cam(c_pos, vec3({2, 3, 2}) - c_pos, {0, 0, 1}, true);

    sf::Clock clock_abs;
    sf::Clock clock_delta;
    double dt;
    sf::Vector2i center(W / 2, H / 2);
    bool space_pressed = false;

    sf::Image im;
    int num_of_obj = 4;
    im.create(3, num_of_obj);
    im.setPixel(0, 0, {0, 0, 0}); //plane y = 0
    im.setPixel(0, 1, {1, 0, 0}); //R
    im.setPixel(0, 2, {1, 0, 0}); //G
    im.setPixel(0, 3, {1, 0, 0}); //b
    im.setPixel(1, 0, {0, 0, 1, 0}); //obj data
    im.setPixel(1, 1, {1, 5, 2, 1});
    im.setPixel(1, 2, {1, 7, 2, 1});
    im.setPixel(1, 3, {3, 6, 2, 1});
    im.setPixel(2, 0, {255, 255, 255}); //color
    im.setPixel(2, 1, {255, 0, 0});
    im.setPixel(2, 2, {0, 255, 0});
    im.setPixel(2, 3, {0, 0, 255});

    sf::Texture obj_data;
    obj_data.loadFromImage(im);
    shader.setUniform("data", obj_data);
    shader.setUniform("num_of_objs", num_of_obj);

    while (window.isOpen()) {
        dt = clock_delta.restart().asSeconds();
        shader.setUniform("iTime", float(clock_abs.getElapsedTime().asSeconds()));
        sf::Event event;
        while (window.pollEvent(event))
            if (event.type == sf::Event::Closed or sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                window.close();
                break;
            }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Q)) {
            cam.rotate_angles(0, -.05, 0);
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::E))
            cam.rotate_angles(0, .05, 0);

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
        cam.move_on_press(dt);
        cam.send_uniforms(shader);

        window.clear();

        window.draw(rect, &shader);
        window.display();
    }
}