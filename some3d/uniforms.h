#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include <iostream>
using std::string;

// if you have no idea what uniforms are:
// uniforms are variables passed to glsl shader(i.e. glsl script) from somewhere outside(c++ code in this case, via sfml).
// This variables are such structures as vectors, matricex, textures, etc.

/* abstract class, which is parent for any object that can send uniforms
*  methods:
*	send_uniforms(shader) -> sends object's uniforms
*/
class UniformsSender {
public:
	virtual void send_uniforms(sf::Shader& shader) const = 0;
};

/* Parent class for any uniform. Used as interface to send data
*  methods:
*	send(shader) -> sends uniform into shader
*  fields:
*	id  -> unique id for each uniform(by which its name in shader is built)
*	type, name, code -> check fields of Uniform class(below) for explanation
*/
class UniformBase {
public:
	static int id;

	string type;
	string name;
	string code;
	virtual void send(sf::Shader&) = 0;
};

/* abstract parent class for structures that can be treated as uniforms(e.g. vec3, vec4). Is actually only a reminder for how to write
*  methods:
*	get_glsl_type() -> returns name of correcponding type in glsl shader
*	uniform()       -> returns sfml uniform(which can then be passed into shader)
*/
template <class T>
class PossibleUniformData {
public:
	string glsl_type; // child class can either initialize glsl_type or override get_slsl_type
	virtual string get_glsl_type() const {
		return glsl_type;
	};
	virtual T uniform() const = 0;
};

/* Template class for uniforms
*  methods:
*	send(shader) -> sends uniform into shader
*  fields:
*	id  -> unique id for each uniform(by which its name in shader is built)
*	T data   -> data of type, that is child of PossibleUniformData
*	type     -> type of data in glsl script
*	name	 -> name of variable in glsl shader related to this uniform
*	code	 -> glsl code for uniform inserted in shader before using the uniform
*/
template <class T>
class Uniform : public UniformBase {
public:
	T data;
	Uniform(T const& data_) {
		data = data_;
		type = data_.get_glsl_type();
		name = "unif_" + std::to_string(id);
		code = "uniform " + type + " " + name + ";\n";
		id += 1;
	}
	Uniform() {}
	void operator=(Uniform<T> const& other) {
		data = other.data;
		type = other.type;
		name = other.name;
		code = other.code;
	}
	void send(sf::Shader& shader) {
		shader.setUniform(name, data.uniform());
	}
};
int UniformBase::id = 0;