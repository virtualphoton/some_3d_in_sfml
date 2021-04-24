#pragma once
#include <string>
#include <SFML/Graphics.hpp>
#include <iostream>

using std::string;

class UniformBase{
public:
	static int id;

	string type;
	string name;
	string code;
	virtual void send(sf::Shader &)=0;
};

template <class T>
class Uniform : public UniformBase{
public:
	T data;
	Uniform(T const & data_){
		data = data_;
		type = data_.glsl_type;
		name = "unif_" + std::to_string(id);
		code = "uniform " + type + " " + name + ";\n";
		id += 1;
	}
	Uniform() {}
	void operator=(Uniform<T> const & other) {
		data = other.data;
		type = other.type;
		name = other.name;
		code = other.code;
	}
	void send(sf::Shader & shader) {
		shader.setUniform(name, data.uniform());
	}
};
int UniformBase::id = 0;