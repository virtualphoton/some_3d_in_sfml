#pragma once
#include "vectors.h"
#include <math.h>
#include <SFML/Graphics.hpp>

class Camera {
public:
	vec3 orig; //origin
	mat3 frame;
	vec3 & right = frame.m1, & forward = frame.m2, & up = frame.m3;
	double sensitivity = 20;
	double zoom = 2;
	Camera(vec3 const & pos_, vec3 const & f_): orig(pos_){
		forward = normalize(f_);
		//default plane is || to Oxy
		right = normalize(cross(forward, vec3(forward.x, forward.y, 0)));
		up = cross(right, forward);
	}

	void rotate_px(double right, double up) {
		rotate_angles(up / 1000 * sensitivity, 0, -right / 1000 * sensitivity);
	}

	void rotate(mat3 const & R) {
		frame = R.T() * frame;
	}
	void rotate_angles(double against_right, double against_forw, double against_up) {
		auto R1 = rot_oz(against_up), R2 = rot_ox(against_right), R3 = rot_oy(against_forw);
		auto R = R1 * R2 * R3;
		rotate(R);
	}
	void set_uniforms(sf::Shader & shader) {
		shader.setUniform("ro", orig.uniform());
		shader.setUniform("f", forward.uniform());
		shader.setUniform("r", right.uniform());
		shader.setUniform("u", up.uniform());
		shader.setUniform("zoom", float(zoom));
	}
};