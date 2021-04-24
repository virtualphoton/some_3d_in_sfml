#pragma once
#include <SFML/Graphics.hpp>
#include "objects3d.h"
#include <vector>
#include <string>
#include "default_shader.h"
#include <set>
#include "default_shader.h"


using std::string;

class Shader {
public:
	bool has_changed = true;
	sf::Shader & compiled_shader = *(new sf::Shader);
	std::vector<Obj3d *> objects;

	Shader & append(Obj3d * obj) {
		objects.push_back(obj);
		has_changed = true;
		return *this;
	}

	string get_color_assemble(string const & body) {
		string start =  "vec3 get_color(vec3 p){\n"
						"float dist, min_dist = 1000000.;\n"
						"vec3 color;\n";
		string end = "return color;\n}\n";
		return start + body + end;
	}
	string get_dist_assemble(string const & body) {
		string start =  "float get_dist(vec3 p){\n"
						"float dist, min_dist = 1000000.;\n";
		string end = "return min_dist;\n}";
		return start + body + end;
	}

	string assemble_shader() {
		string uniforms;
		string funcs;
		string get_color_body;
		string get_dist_body;
		std::set<string> used_types;
		for (Obj3d * obj : objects) {
			uniforms.append("uniform " + obj->uniform_type + " " + obj->uniform_data_name + ";\n");
			uniforms.append("uniform vec3 " + obj->uniform_color_name + ";\n");

			if (used_types.find(obj->type) == used_types.end()) {
				funcs.append(obj->code).append("\n");
				used_types.insert(obj->type);
			}

			get_dist_body.append("dist = " + obj->type + "_dist(p, " + obj->uniform_data_name + ");\n")
						 .append("if (dist < min_dist)\n"
						 		 "    min_dist = dist;\n");
			get_color_body.append("dist = " + obj->type + "_dist(p, " + obj->uniform_data_name + ");\n")
						  .append("if (dist < min_dist){\n"
								  "    min_dist = dist;\n"
								  "    color = " + obj->uniform_color_name + ";\n"
								  "}\n");
		}
		return default_shader.body + uniforms + funcs + get_color_assemble(get_color_body) + get_dist_assemble(get_dist_body);
	}

	sf::Shader & get_shader() {
		if (has_changed){
			string some = assemble_shader();
			std::cout << some;
			compiled_shader.loadFromMemory(some, sf::Shader::Fragment);
			has_changed = false;
		}
		return compiled_shader;
	}

	void send_uniforms() {
		for (Obj3d * obj : objects)
			obj->send_uniforms(compiled_shader);
	}
};