#pragma once
#include "vectors.h"
#include <math.h>
#include <SFML/Graphics.hpp>
#include "useful_funcs.h"


double pi = 3.14159265;

class Camera {
public:
	vec3 orig; //origin
	mat3 frame;
	vec3 & right = frame.m1, & forward = frame.m2, & up = frame.m3;
	double sensitivity = 20;
	double sens_mul = 1 / 10000.;
	double zoom = 1024./768;
	vec3 fixed_up;
	bool up_is_fixed = false;

	Camera(vec3 const & pos_, vec3 const & f_, vec3 const& up_, bool up_is_fixed_=false): orig(pos_){
		up_is_fixed = up_is_fixed_;

		forward = normalize(f_);
		fixed_up = normalize(up_);
		right = cross(forward, fixed_up);
		up = cross(right, forward);
	}

	void rotate(mat3 const & R) {
		frame = (R * frame.T()).T();
	}
	void rotate_px(double r, double u) {
		double r_phi = -r * sens_mul * sensitivity;
		double u_phi = u * sens_mul * sensitivity;
		if (not up_is_fixed)
			rotate_angles(u_phi, 0, r_phi);
		else {
			mat3 R = rot(fixed_up, r_phi);
			rotate(R);
			double forw_angle = acos(dot(fixed_up, forward)); //angle between up_fixed and forward
			rotate_angles(forw_angle - clamp(forw_angle - u_phi, .01, pi - .01), 0, 0);
		}
	}
	void rotate_angles(double against_right, double against_forw, double against_up) {
		auto R1 = rot(up, against_up), R2 = rot(right, against_right), R3 = rot(forward, against_forw);
		auto R = R1 * R2 * R3;
		rotate(R);
	}

	void move(double dr, double df, double du) {
		orig += dr;
		if (up_is_fixed) {

		}
	}

	void send_uniforms(sf::Shader & shader) {
		shader.setUniform("ro", orig.uniform());
		shader.setUniform("f", forward.uniform());
		shader.setUniform("r", right.uniform());
		shader.setUniform("u", up.uniform());
		shader.setUniform("zoom", float(zoom));
	}
};