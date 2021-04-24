#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include "vectors.h"
#include <iostream>

using std::string;

class Obj3d {
public:
	static int id;
	string type;
	string code;
	string uniform_data_name;
	string uniform_type;
	vec3 color;
	string uniform_color_name;

	Obj3d() {
		uniform_data_name = "obj3d_" + std::to_string(id);
		uniform_color_name = "color_" + std::to_string(id);
		id += 1;
	}
	virtual void send_uniforms(sf::Shader & shader) = 0;
};
int Obj3d::id = 0;

class Obj3d_vec4:public Obj3d {
public:
	vec4 data;
	Obj3d_vec4(vec4 const & data_, vec3 const & color_):data(data_){
		color = color_;
		uniform_type = "vec4";
	};
	virtual void send_uniforms(sf::Shader & shader) {
		shader.setUniform(uniform_data_name, data.uniform());
		shader.setUniform(uniform_color_name, color.uniform());
	}
};

class Plane: public Obj3d_vec4 {
public:
	Plane(vec4 const & data_, vec3 const & color_):Obj3d_vec4(data_, color_) {
		type = "plane";
		code =  "float plane_dist(vec3 p, vec4 plane) {"
				"	return dot(vec4(p, 1), plane) / length(plane.xyz);"
				"}";
	}
};

class Sphere: public Obj3d_vec4 {
public:
	Sphere(vec4 const & data_, vec3 const & color_):Obj3d_vec4(data_, color_) {
		type = "sphere";
		code =  "float sphere_dist(vec3 p, vec4 s) {"
				"    return length(p-s.xyz)-s.w;"
				"}";
	}
};