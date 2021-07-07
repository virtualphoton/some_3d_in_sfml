#pragma once
#include "../objects2d.h"

void draw_line(sf::Image & im, int x_s, int y_s, int x_e, int y_e, vec3 const & line_color, vec3 const & constraints, double thickness);
void draw_circle(sf::Image & im, vec3 const & center, vec3 const & line_color, vec3 const & constraints, double thickness);
void draw_thin_line(sf::Image & im, int x0, int y0, int x1, int y1, vec3 const & line_color, vec3 const & constraints, bool aa);
bool check_constr(int x, int y, vec3 const & constraints) {
    return x >= 0 and x <= constraints.x and y >= 0 and y <= constraints.y;
}

class Level1 {
public:
    const int W, H;

    Level1(int W_, int H_): W(W_), H(H_) {}

    void run() {
        sf::RenderWindow window(sf::VideoMode(W, H), "", sf::Style::Close);
        window.setFramerateLimit(60);
        sf::RectangleShape rect(sf::Vector2f(W, H));
        Shader sh;

        Plane plane_obj({0,0,1,0}, {1, 1, 1});
        Box box_obj({4,4,4}, {10, .3, 4}, {0, 1, 1});
        box_obj.color_func = "texture(drawing, vec2(1, 0) + vec2(-1, 1)*(p.xz-vec2(-6, 0))/vec2(20, 8)).xyz";
        sh.append(&plane_obj, &box_obj);

        sh.add_uniform("uniform sampler2D drawing;");

        sf::Shader & shader = sh.compile();
        sh.send_uniforms();

        Rectangle drawing_rect({-6, 4.3, 0}, {20, 0, 0}, {0, 0, 8});
        sf::Image drawing;
        drawing.create(1024, 409, sf::Color(255, 255, 255));
        sf::Texture texture;
        texture.loadFromImage(drawing);


        shader.setUniform("light_pos", vec3(4, 10, 4).uniform());

        vec3 c_pos(6, 10.5, 3.3);
        Camera cam(c_pos, vec3({6, 4.5, 3.3}) - c_pos, {0, 0, 1}, true);

        sf::Clock clock_abs;
        sf::Clock clock_delta;
        double dt;
        sf::Vector2i center(W / 2, H / 2);
        shader.setUniform("iResolution", sf::Vector2<float>(W, H));
        bool space_pressed = false;
        bool in_focus = true;
        int i = 0;
        int x_prev = -1, y_prev = -1;
        bool is_repeat = false;
        while (window.isOpen()) {
            //std::cout << cam.orig.x << ' ' << cam.orig.y << ' ' << cam.orig.z << '\n';
            shader.setUniform("iTime", float(clock_abs.getElapsedTime().asSeconds()));
            dt = clock_delta.restart().asSeconds();
            sf::Event event;
            while (window.pollEvent(event)) {
                if (event.type == sf::Event::Closed or sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
                    window.close();
                    return;
                } else if (event.type == sf::Event::LostFocus)
                    in_focus = false;
                else if (event.type == sf::Event::GainedFocus)
                    in_focus = true;
            }

            if (in_focus) {
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
                    if (space_pressed) {
                        auto cur_shift = sf::Mouse::getPosition(window) - center;
                        cam.rotate_px(cur_shift.x, -cur_shift.y);
                    } else
                        space_pressed = true;
                    sf::Mouse::setPosition(center, window);

                    window.setMouseCursorVisible(false);
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Left) or sf::Mouse::isButtonPressed(sf::Mouse::Right)) {
                        double size;
                        vec3 color;
                        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                            size = 6.5;
                            color = {0., 0, 0};
                        } else {
                            size = 10.5;
                            color = {1., 1, 1};
                        }
                        vec3 inters = drawing_rect.intersect(cam.orig, cam.forward);
                        if (inters) {
                            int x = 1023 - clench(inters.x, -6, 14, 0, 1023), y = clench(inters.z, 0, 8, 0, 408);
                            if (x_prev != -1) {
                                bool is_same = x == x_prev and y == y_prev;
                                if (not is_same or is_same and not is_repeat) {
                                    draw_line(drawing, x_prev, y_prev, x, y, color, {1023., 408, 0}, size);
                                    x_prev = x;
                                    y_prev = y;
                                    texture.loadFromImage(drawing);
                                }

                                if (not is_same)
                                    is_repeat = false;
                                if (is_same and not is_repeat)
                                    is_repeat = true;
                            } else {
                                is_repeat = false;
                                x_prev = x;
                                y_prev = y;
                            }
                        }
                    } else {
                        x_prev = -1;
                    }
                } else {
                    space_pressed = false;
                    window.setMouseCursorVisible(true);
                }
                cam.move_on_press(dt);
            }

            shader.setUniform("drawing", texture);
            cam.send_uniforms(shader);

            window.clear();
            window.draw(rect, &shader);
            window.display();
        }
	}
};

void draw_line(sf::Image & im, int x_s, int y_s, int x_e, int y_e, vec3 const & line_color, vec3 const & constraints, double thickness) {
    draw_circle(im, vec3(x_s, y_s, 0), line_color, constraints, thickness);
    draw_circle(im, vec3(x_e, y_e, 0), line_color, constraints, thickness);

    vec3 normal = normalize(vec3(-(y_e - y_s), x_e - x_s, 0)) * thickness;
    bool steep = abs(y_e - y_s) > abs(x_e - x_s);
    //borders of line segment consisting of starts of parallel lines
    int x0 = x_s - normal.x, y0 = y_s - normal.y;
    int x1 = x_s + normal.x, y1 = y_s + normal.y;

    draw_thin_line(im, x0, y0, x0 + (x_e - x_s), y0 + (y_e - y_s), line_color, constraints, true);
    draw_thin_line(im, x1, y1, x1 + (x_e - x_s), y1 + (y_e - y_s), line_color, constraints, true);
    if (not steep) {
        int dy = sign(y1 - y0);
        for (int y = y0 + dy; y != y1; y += dy) {
            draw_thin_line(im, x0, y, x0 + (x_e - x_s), y + (y_e - y_s), line_color, constraints, false);
            draw_thin_line(im, x1, y, x1 + (x_e - x_s), y + (y_e - y_s), line_color, constraints, false);
        }
    } else {
        int dx = sign(x1 - x0);
        for (int x = x0 + dx; x != x1; x += dx) {
            draw_thin_line(im, x, y0, x + (x_e - x_s), y0 + (y_e - y_s), line_color, constraints, false);
            draw_thin_line(im, x, y1, x + (x_e - x_s), y1 + (y_e - y_s), line_color, constraints, false);
        }
    }
}

void draw_thin_line(sf::Image & im, int x0, int y0, int x1, int y1, vec3 const & line_color, vec3 const & constraints, bool aa) {
    //https://www.geeksforgeeks.org/anti-aliased-line-xiaolin-wus-algorithm/
    sf::Color line_col = line_color.color();
    bool steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        std::swap(x0, y0);
        std::swap(x1, y1);
    }
    if (x0 > x1) {
        std::swap(x0, x1);
        std::swap(y0, y1);
    }
    bool up = y1 >= y0;

    double dx = x1 - x0;
    double dy = y1 - y0;
    double grad = dy / dx;
    if (dx == 0.0)
        grad = 1;
    double y = y0;
    int x_px, y_px;
    int y_px_t, y_px_b;
    for (int x = x0; x <= x1; x += 1) {
        x_px = x;
        y_px = int(y+.5);
        y_px_t = int(y) + 1;
        y_px_b = int(y);
        if (aa) {
            if (not steep) {
                if (check_constr(x_px, y_px_b, constraints))
                    im.setPixel(x_px, y_px_b, line_color.color_over(im.getPixel(x_px, y_px_b), 1 - (y - int(y))));

                if (check_constr(x_px, y_px_t, constraints))
                    im.setPixel(x_px, y_px_t, line_color.color_over(im.getPixel(x_px, y_px_t), y - int(y)));
            } else {
                if (check_constr(y_px_b, x_px, constraints))
                    im.setPixel(y_px_b, x_px, line_color.color_over(im.getPixel(y_px_b, x_px), 1 - (y - int(y))));

                if (check_constr(y_px_t, x_px, constraints))
                    im.setPixel(y_px_t, x_px, line_color.color_over(im.getPixel(y_px_t, x_px), y - int(y)));
            }
        } else {
            if (not steep) {
                if (check_constr(x_px, y_px, constraints))
                    im.setPixel(x_px, y_px, line_col);
            } else {
                if (check_constr(y_px, x_px, constraints))
                    im.setPixel(y_px, x_px, line_col);
            }
        }
        y += grad;
    }
}

void draw_circle(sf::Image & im, vec3 const & center, vec3 const & line_color, vec3 const & constraints, double thickness) {
    //draws filled circle with antialiazing
    sf::Color line_col = line_color.color();
    int y_, x_, x_l, x_r;
    double x_offset, err;
    //part of circle without 1 layer on top and on bottom, because they're mostly transparent
    for (double y = -thickness+1; y < thickness; y += 1) {
        y_ = center.y + y;
        x_offset = sqrt(thickness * thickness - y * y);
        err = 1+int(x_offset) - x_offset;
        x_r = center.x + int(x_offset);
        x_l = center.x - int(x_offset);
        if (check_constr(x_r, y_, constraints))
            im.setPixel(x_r, y_, line_color.color_over(im.getPixel(x_r, y_), 1 - err));
        if (check_constr(x_l, y_, constraints))
            im.setPixel(x_l , y_, line_color.color_over(im.getPixel(x_l, y_), 1 - err));
        x_l = max(x_l+1, 0);
        x_r = min(x_r-1, constraints.x);
        for (int x = x_l; x <= x_r; x += 1)
            if (check_constr(x, y_, constraints))
                im.setPixel(x, y_, line_col);
    }
    int y_t, y_b;
    y_t = center.y + thickness;
    y_b = center.y - thickness;
    x_offset = sqrt(.5 * thickness - .25);
    //transparent layers on top and bottom
    for (int x = -x_offset; x <= x_offset; x += 1) {

        double y = sqrt(thickness * thickness - x * x);
        err = 1 + int(y) - y;
        if (abs(x / x_offset) < .5 and abs(err - 1) < .01)
            err = 0;
        x_ = center.x + x;
        if (check_constr(x_, y_t, constraints))
            im.setPixel(x_, y_t, line_color.color_over(im.getPixel(x_, y_t), 1-err));
        if (check_constr(x_, y_b, constraints))
            im.setPixel(x_, y_b, line_color.color_over(im.getPixel(x_, y_b), 1-err));
    }
}