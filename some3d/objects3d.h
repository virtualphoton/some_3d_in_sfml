#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include <iostream>
#include <vector>
#include "vectors.h"
#include "uniforms.h"
#include "useful_funcs.h"
using std::string;


/* parent class for 3d objects, i.e. those that will be rendered in sfml shader
*  methods:
*	add_uniforms(uniforms*...) -> adds uniforms that will be latersend to shader
*	send_uniforms()	           -> sends uniforms
*	compose_uniforms_code()    -> string of code for uniforms that will be inserted into glsl script
*	compose_get_dist()         -> string of code that will be inserted inside glsl script after `dist = ` (so it usually contains name of distance function and params to call it)
*  fields:
*	string type				   -> type of 3d objects
*	string distance_func_code  -> code to calculate signed distance, usually is the default code for each child's class
*	Uniform<vec3> color		   -> uniform containing rgb color, because by default object will have this color
*	string color_func_code     -> code for color function. Is piece of code inserted in `vec3 color = <...>;`. In code is available variable `p`, vec3 position of ray
*								  collision with object. By default function is assigned to name of `color` uniform(i.e. object is monochrome)
*  
*  children of Obj3d have one or more mutable Uniforms containing data about them(e.g. position)
*  signed distance functions(SDFs) for basic shapes are here: https://www.iquilezles.org/www/articles/distfunctions/distfunctions.htm
*/
class Obj3d : public UniformsSender {
public:
	string type;
	string distance_func_code;
	string color_func_code;
	Uniform<vec3> color;

	Obj3d(vec3 const& color_):color(color_){
		color_func_code = color.name;
		add_uniforms(&color);
	}
	std::vector<UniformBase*> _uniforms;
	//all uniforms by default are stored in `uniforms` vector
	virtual void send_uniforms(sf::Shader& shader) const override {
		for (UniformBase* unif : _uniforms)
			unif->send(shader);
	}
	virtual string compose_uniforms_code() {
		string out = "";
		for (UniformBase* unif : _uniforms)
			out.append(unif->code);
		return out;
	}
	virtual string compose_get_dist() = 0;

	template <class Head>
	void add_uniforms(Head* obj) {
		_uniforms.push_back(obj);
	}
	template <class Head, class ...Tail>
	void add_uniforms(Head* obj, Tail * ... args) {
		_uniforms.push_back(obj);
		add_uniforms(args...);
	}
};

/* class for plane 3d object(though technically it is not). It is defined by equation Ax+By+Cz+D=0 and fills all negative half-space
*  uniforms:
*	<vec4> data -> contains A, B, C and D coefficients from plane equation Ax+By+Cz+D=0
*/
class Plane : public Obj3d {
public:
	Uniform<vec4> data;
	Plane(vec4 const& data_, vec3 const& color_=null_vec3):Obj3d(color_), data(data_){
		type = "plane";
		distance_func_code = "float plane_dist(vec3 p, vec4 plane) {\n"
							 "	 return dot(vec4(p, 1), plane) / length(plane.xyz);\n"
							 "}\n";
		add_uniforms(&data);
	}
	string compose_get_dist() override {
		return format("    dist = {}_dist(p, {});\n", type, data.name);
	}
};

/* defines filled sphere
*  uniforms:
*	<vec4> data -> contains x,y,z coords and r radius
*/
class Sphere : public Obj3d {
public:
	Uniform<vec4> data;
	Sphere(vec4 const& data_, vec3 const& color_ = null_vec3) :Obj3d(color_), data(data_) {
		type = "sphere";
		distance_func_code = "float sphere_dist(vec3 p, vec4 s) {\n"
							 "    return length(p-s.xyz)-s.w;\n"
							 "}\n";
		add_uniforms(&data);
	}
	string compose_get_dist() override {
		return format("    dist = {}_dist(p, {});\n", type, data.name);
	}
};

/* defines Box(Parallelepiped) with sides parallel to axes
*  uniforms:
*	<vec3> center_pos	  -> x,y,z position of center
*	<vec3> half_edges_len -> half lengths of sides parallel to Ox, Oy and Oz respectively
*	TODO rotation matrix
*/
class Box : public Obj3d {
public:
	Uniform<vec3> center_pos;
	Uniform<vec3> half_edges_len;
	Box(vec3 const& data0, vec3 const& data1, vec3 const& color_ = null_vec3):Obj3d(color_), center_pos(data0), half_edges_len(data1){
		type = "simple_cube";
		distance_func_code = "float simple_cube_dist(vec3 p, vec3 center, vec3 restraints) {\n"
							 "	 vec3 q = abs(p-center) - restraints;"
					 		 "    return length(max(q, 0)) + min(max(q.x,max(q.y,q.z)),0.0);\n"
						 	 "}\n";
		add_uniforms(&center_pos, &half_edges_len);
	}
	string compose_get_dist() override {
		return format("    dist = {}_dist(p, {}, {});\n", type, center_pos.name, half_edges_len.name);
	}
};
