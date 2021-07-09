#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "vectors.h"
#include "uniforms.h"
#include "useful_funcs.h"

using std::string;

class Obj3d : public UniformsSender {
public:

	string type;
	string code;

	std::vector<UniformBase*> uniforms;
	string color_func;

	virtual string compose_get_dist() = 0;

	virtual void send_uniforms(sf::Shader& shader) const override {
		for (UniformBase* unif : uniforms)
			unif->send(shader);
	}
	virtual string compose_uniforms_code() {
		string out = "";
		for (UniformBase* unif : uniforms)
			out.append(unif->code);
		return out;
	}

	template <class Head>
	void push_back(Head* obj) {
		uniforms.push_back(obj);
	}
	template <class Head, class ...Tail>
	void push_back(Head* obj, Tail * ... args) {
		uniforms.push_back(obj);
		push_back(args...);
	}
};

class Plane : public Obj3d {
public:
	Uniform<vec4> data;
	Uniform<vec3> color;
	Plane(vec4 const& data_, vec3 const& color_) {
		type = "plane";
		code = "float plane_dist(vec3 p, vec4 plane) {\n"
			"	return dot(vec4(p, 1), plane) / length(plane.xyz);\n"
			"}\n";
		data = data_;
		color = color_;
		push_back(&data, &color);
		color_func = color.name;
	}
	string compose_get_dist() {
		return format("    dist = {}_dist(p, {});\n", type, data.name);
	}
};

class Sphere : public Obj3d {
public:
	Uniform<vec4> data;
	Uniform<vec3> color;
	Sphere(vec4 const& data_, vec3 const& color_) {
		type = "sphere";
		code = "float sphere_dist(vec3 p, vec4 s) {\n"
			"    return length(p-s.xyz)-s.w;\n"
			"}\n";

		data = data_;
		color = color_;
		push_back(&data, &color);
		color_func = color.name;
	}
	string compose_get_dist() {
		return format("    dist = {}_dist(p, {});\n", type, data.name);
	}
};

class Box : public Obj3d {
public:
	Uniform<vec3> center_pos;
	Uniform<vec3> half_edges_len;
	Uniform<vec3> color;
	Box(vec3 const& data0, vec3 const& data1, vec3 const& color_) {
		type = "simple_cube";
		code = "float simple_cube_dist(vec3 p, vec3 center, vec3 restraints) {\n"
			"	 vec3 q = abs(p-center) - restraints;"
			"    return length(max(q, 0)) + min(max(q.x,max(q.y,q.z)),0.0);\n"
			"}\n";

		center_pos = data0;
		half_edges_len = data1;
		color = color_;
		push_back(&center_pos, &half_edges_len, &color);
		color_func = color.name;
	}
	string compose_get_dist() {
		return format("    dist = {}_dist(p, {}, {});\n", type, center_pos.name, half_edges_len.name);
	}
};
