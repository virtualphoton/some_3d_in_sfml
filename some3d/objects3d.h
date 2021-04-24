#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "vectors.h"
#include "uniforms.h"
#include "useful_funcs.h"

using std::string;

class Obj3d {
public:

	string type;
	string code;

	std::vector<UniformBase*> uniforms;
	string color_func;

	virtual string compose_get_dist() = 0;

	virtual void send_uniforms(sf::Shader & shader) {
		for (UniformBase * unif : uniforms)
			unif->send(shader);
	}
	virtual string compose_uniforms_code() {
		string out = "";
		for (UniformBase * unif : uniforms)
			out.append(unif->code);
		return out;
	}
};

class Plane: public Obj3d{
public:
	Uniform<vec4> data;
	Uniform<vec3> color;
	Plane(vec4 const & data_, vec3 const & color_){
		type = "plane";
		code =  "float plane_dist(vec3 p, vec4 plane) {\n"
				"	return dot(vec4(p, 1), plane) / length(plane.xyz);\n"
				"}\n";
		data = data_;
		color = color_;
		uniforms.push_back(&data);
		uniforms.push_back(&color);
		color_func = color.name;
	}
	string compose_get_dist() {
		return format("    dist = {}_dist(p, {});\n", type, data.name);
	}
};

class Sphere: public Obj3d {
public:
	Uniform<vec4> data;
	Uniform<vec3> color;
	Sphere(vec4 const & data_, vec3 const & color_){
		type = "sphere";
		code =  "float sphere_dist(vec3 p, vec4 s) {\n"
				"    return length(p-s.xyz)-s.w;\n"
				"}\n";

		data = data_;
		color = color_;
		uniforms.push_back(&data);
		uniforms.push_back(&color);
		color_func = color.name;
	}
	string compose_get_dist() {
		return format("    dist = {}_dist(p, {});\n", type, data.name);
	}
};