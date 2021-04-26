#pragma once
#include "../objects2d.h"

void draw_line(sf::Image & im, int x_s, int y_s, int x_e, int y_e, vec3 const & line_color, vec3 const & constraints, double thickness);
void draw_circle(sf::Image & im, vec3 const & center, vec3 const & line_color, vec3 const & constraints, double thickness);
void draw_thin_line(sf::Image & im, int x0, int y0, int x1, int y1, vec3 const & line_color, vec3 const & constraints, bool aa);

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
        Box box_obj({4,4,4}, {10, .1, 4}, {0, 1, 1});
        box_obj.color_func = "texture(drawing, (p.xz-vec2(-6, 0))/vec2(20, 8)).xyz";
        sh.append(&plane_obj, &box_obj);

        sh.add_uniform("uniform sampler2D drawing;");

        sf::Shader & shader = sh.compile();
        sh.send_uniforms();

        Rectangle drawing_rect({-6, 4.1, 0}, {20, 0, 0}, {0, 0, 8});
        sf::Image drawing;
        drawing.create(1024, 409, sf::Color(255, 255, 255));
        sf::Texture texture;
        draw_line(drawing, 100, 100, 210, 300, {0, 0, 0}, {1023., 408., 0}, 20);
        //drawing.setPixel(40, 40, sf::Color::Green);
        texture.loadFromImage(drawing);


        shader.setUniform("light_pos", vec3(4, 10, 4).uniform());

        vec3 c_pos(-2, 8, 4);
        Camera cam(c_pos, vec3({-2, 4, 4}) - c_pos, {0, 0, 1}, true);

        sf::Clock clock_abs;
        sf::Clock clock_delta;
        double dt;
        sf::Vector2i center(W / 2, H / 2);
        shader.setUniform("iResolution", sf::Vector2<float>(W, H));
        bool space_pressed = false;
        bool in_focus = true;
        int i = 0;

        while (window.isOpen()) {
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
                    if (sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
                        vec3 inters = drawing_rect.intersect(cam.orig, cam.forward);
                        if (inters) {
                            int x = clench(inters.x, -6, 14, 0, 1023), y = clench(inters.z, 0, 8, 0, 408);
                            draw_circle(drawing, vec3(x, y, 0), {0, 0, 0}, {1023, 408, 0}, 20.5);

                            texture.loadFromImage(drawing);
                        }
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
    //draw line without smooth ends through (x_s, y_s) and (x_e, y_e)
    vec3 normal = normalize(vec3(-(y_e - y_s), x_e - x_s, 0)) * thickness;

    //borders of line segment consisting of starts of parallel lines
    int x0 = x_s - normal.x, y0 = y_s - normal.y;
    int x1 = x_s + normal.x, y1 = y_s + normal.y;

    //we go through line segment from (x0, y0) to (x1, y1) and draw parallel line through every pixel
    //algorithm is similar to draw_thin_line with no antialiazing, but we draw line instead of pixel
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
    for (int x = x0; x <= x1; x += 1) {
        if (steep) {
            x_px = y;
            y_px = x;
        } else {
            x_px = x;
            y_px = y;
        }
        bool smooth = x == x0 or x == x1;
        //draw parallel lines through x_px and y_py. if it's border, then use antialiazing
        draw_thin_line(im, x_px, y_px, x_px + (x_e - x_s), y_px + (y_e - y_s), line_color, constraints, 1);
        /*x_px += steep ? (up ? 1 : -1) : 0;
        y_px += steep ? 0 : (up ? 1 : -1);
        draw_thin_line(im, x_px, y_px, x_px + (x_e - x_s), y_px + (y_e - y_s), line_color, constraints, 0);*/
        y += grad;
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
    for (int x = x0; x <= x1; x += 1) {
        if (steep) {
            x_px = y;
            y_px = x;
        } else {
            x_px = x;
            y_px = y;
        }
        if (aa) {
            if (x_px >= 0 and x_px <= constraints.x and y_px >= 0 and y_px <= constraints.y)
                im.setPixel(x_px, y_px, line_color.color_over(im.getPixel(x_px, y_px), 1 - (y - int(y))));
            x_px += steep ? (up? 1:-1) : 0;
            y_px += steep ? 0 : (up ? 1:-1);
            if (x_px >= 0 and x_px <= constraints.x and y_px >= 0 and y_px <= constraints.y)
                im.setPixel(x_px, y_px, line_color.color_over(im.getPixel(x_px, y_px), y - int(y)));
        } else {
            if (x_px >= 0 and x_px <= constraints.x and y_px >= 0 and y_px <= constraints.y)
                im.setPixel(x_px, y_px, line_col);
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
        if (y_ < 0 or y_ > constraints.y)
            continue;
        x_offset = sqrt(thickness * thickness - y * y);
        err = 1+int(x_offset) - x_offset;
        x_r = center.x + int(x_offset);
        x_l = center.x - int(x_offset);
        if (x_r <= constraints.x)
            im.setPixel(x_r, y_, line_color.color_over(im.getPixel(x_r, y_), 1 - err));
        if (x_l >= 0)
            im.setPixel(x_l , y_, line_color.color_over(im.getPixel(x_l, y_), 1 - err));
        x_l = max(x_l+1, 0);
        x_r = min(x_r-1, constraints.x);
        for (int x = x_l; x <= x_r; x += 1)
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
        if (x_ < 0 or x_ > constraints.x)
            continue;
        if (y_t <= constraints.y)
            im.setPixel(x_, y_t, line_color.color_over(im.getPixel(x_, y_t), 1-err));
        if (y_b >= 0)
            im.setPixel(x_, y_b, line_color.color_over(im.getPixel(x_, y_b), 1-err));
    }
}