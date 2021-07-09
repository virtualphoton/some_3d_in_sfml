#pragma once
#include <math.h>
#include <SFML/Graphics.hpp>
#include "vectors.h"
#include "useful_funcs.h"
#include "uniforms.h"
#include <map>
#include <string>

double pi = 3.14159265;

/* structure containing data about basic camera movement
*  fields:
*    speed
*    bindings  - map of {sf::Keyboard::Key : vector}, where vector is a 3d-vector, which coords show how far to move in right, forward and upward directions respectively
*				(those vectors direction depend on whether upward vector of camera is fixed or not)
*/
#define KB(Symbol) sf::Keyboard::Symbol
struct move_info_struct {
	double speed = 10;
	std::map<sf::Keyboard::Key, vec3> bindings;
	move_info_struct() {
		// {key_idx: (right, forw, up)}
		bindings[KB(D)] = { 1, 0, 0 };
		bindings[KB(A)] = { -1, 0, 0 };
		bindings[KB(W)] = { 0, 1, 0 };
		bindings[KB(S)] = { 0, -1, 0 };
		bindings[KB(LShift)] = { 0, 0, 1 };
		bindings[KB(LControl)] = { 0, 0, -1 };
	}
};

/* camera class of player.
*  consrtuctors:
*	(camera position, forward vector, upward vector, up_is_fixed) - first param has "origin" alias, the next 2 define orientation of camera. 
*												up_is_fixed defines whether player's up is fixed(e.g. as in shooter) or not(as if in space).
*  methods:
*	rotate(mat3 R)                     -> rotate using rotate matrix R
*	rotate_px(to_the_right, to_the_up) -> rotate, when player moves mouse by several pixels. Angle's defined by sensitivity field(takes into account up_is_fixed field)
*   rotate_angles(against right, against forward, against up) -> rotates against corresponding axis in same order as parameters go
*			(! idk, if I should keep it, because it might only be useful only for small angles and rotation against one axis, because of order of rotations)
*	move_on_press(delta time)          -> checks if user pressed keys(from move_info_struct) and changes camera position according to time passed into it
*	send_uniform(shader)               -> send uniforms
*  fields:
*	vec3 orig          -> original of camera, i.e. its position
*	mat3 frame         -> 3x3 matrix, which rows correspond to right, forward, up vectors
*	right, forward, up -> links to vectors from frame
*	double sensitivity -> sensitivity of camera, multiplied then by SENS_MUL
*	double zoom        -> distance between camera origin and rectangle, on which all images will project(its center will have coords of orig+forward*zoom)
*	bool up_is_fixed   -> shows if there's a global upward vector(explained in constructor)
*	   vec3 fixed_up   -> gloabal upwards vector
*/
class Camera : public UniformsSender {
public:
	vec3 orig; //origin(where camera is right now)
	mat3 frame;
	vec3& right = frame.m1, & forward = frame.m2, & up = frame.m3;
	double sensitivity = 20;      //changeable parameter of sensitivity
	double SENS_MUL = 1 / 10000.; // constant multiplier for sensitivity
	double zoom = .9;
	vec3 fixed_up;
	bool up_is_fixed = false;
	move_info_struct move_info;

	Camera(vec3 const& pos, vec3 const& f_, vec3 const& up_, bool up_is_fixed = false) :orig(pos), up_is_fixed(up_is_fixed){
		forward = normalize(f_);
		fixed_up = normalize(up_);
		right = normalize(cross(forward, fixed_up));
		up = normalize(cross(right, forward));
	}

	void rotate(mat3 const& R) {
		// this comes from fact that vec_rotated = R * vec, i.e. vec is colum => frame = (R*frame.T).T = frame*R.T
		frame = frame * R.T();
		right = normalize(right);
		forward = normalize(forward);
		up = normalize(up);
	}
	void rotate_px(double r, double u) {
		double r_phi = -r * SENS_MUL * sensitivity;
		double u_phi = u * SENS_MUL * sensitivity;
		if (not up_is_fixed)
			rotate_angles(u_phi, 0, r_phi);
		else {
			double forw_angle = acos(dot(fixed_up, forward)); //angle between up_fixed and forward
			rotate_angles(forw_angle - clamp(forw_angle - u_phi, .02, pi - .02), 0, 0);
			mat3 R = rot(fixed_up, r_phi);
			rotate(R);
		}
	}
	void rotate_angles(double against_right, double against_forw, double against_up) {
		auto R1 = rot(up, against_up), R2 = rot(right, against_right), R3 = rot(forward, against_forw);
		auto R = R2 * R3 * R1;
		rotate(R);
	}
	void move_on_press(double dt) {
		for (auto& elem : move_info.bindings)
			if (sf::Keyboard::isKeyPressed(elem.first)) {
				mat3 fixed_up_frame = mat3(right, cross(fixed_up, right), fixed_up);
				orig += move_info.speed * elem.second * (up_is_fixed ? fixed_up_frame : frame) * dt;
			}

	}

	void send_uniforms(sf::Shader& shader) const override{
		shader.setUniform("ro", orig.uniform());
		shader.setUniform("f", forward.uniform());
		shader.setUniform("r", right.uniform());
		shader.setUniform("u", up.uniform());
		shader.setUniform("zoom", float(zoom));
	}

};