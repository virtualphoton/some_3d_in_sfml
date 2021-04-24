#include <SFML/Graphics.hpp>
#include <iostream>
#include "camera.h"
#include "vectors.h"
#include "default_shader.h"
#include <string>
#include "shader.h"
#include "objects3d.h"
#include "uniforms.h"
#include "useful_funcs.h"

const int W = 1024;
const int H = 768;

using std::vector;
using std::string;
int main() {

    sf::RenderWindow window(sf::VideoMode(W, H), "", sf::Style::Close);
    window.setFramerateLimit(60);
    sf::RectangleShape rect(sf::Vector2f(W, H));
    Shader sh;
    Plane plane_obj({0,0,1,0}, {.8, .8, .8});
    plane_obj.color_func = "texture(tex, fract(p.xy/5*textureSize(tex, 0).y/textureSize(tex, 0).x)).xyz";
    //plane_obj.color_func = "(clamp(rnd_(p.xy*10), 0, 1) + .5)*vec3(194,178,128)/255";
    Sphere sph_obj({1,2,4,1}, {1, 0, 0});
    Sphere sph_obj2({2.5,2,4,1}, {0, 1, 0});
    Sphere sph_obj3({1.5,2.7,4,1}, {0, 0, 1});
    Sphere sph_obj4({3,2.7,4,1}, {0, 1, 1});
    sh.append(&plane_obj, &sph_obj/*, &sph_obj2, &sph_obj3*/);
    sh.add_func(get_rnd_f()).add_func(get_voronoy_f());
    sh.add_uniform("uniform sampler2D tex;");

    sf::Shader & shader = sh.compile();
    sh.send_uniforms();

    sf::Texture texture;
    texture.loadFromFile("Textures/sand.jpg");
    shader.setUniform("tex", texture);

    vec3 c_pos(-2, 6, 20);
    Camera cam(c_pos, vec3({2, 3, 2}) - c_pos, {0, 0, 1}, true);
    cam.rotate_px(0, -786);


    sf::Clock clock_abs;
    sf::Clock clock_delta;
    double dt;
    sf::Vector2i center(W / 2, H / 2);
    bool space_pressed = false;
    bool in_focus = true;

    while (window.isOpen()) {
        shader.setUniform("iResolution", sf::Vector2<float>(W, H));
        shader.setUniform("iTime", float(clock_abs.getElapsedTime().asSeconds()));
        dt = clock_delta.restart().asSeconds();
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed or sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                window.close();
                break;
            } else if (event.type == sf::Event::LostFocus) {
                in_focus = false;
            } else if (event.type == sf::Event::GainedFocus) {
                in_focus = true;
            }
        }

        if (in_focus) {
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
        }

        cam.send_uniforms(shader);

        window.clear();

        window.draw(rect, &shader);
        window.display();
    }
}