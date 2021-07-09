#pragma once
#include <SFML/Graphics.hpp>
#include "objects3d.h"
#include <vector>
#include <string>
#include "default_shader.h"
#include <set>


using std::string;

class Shader {
public:
	bool has_changed = true;
	sf::Shader& compiled_shader = *(new sf::Shader);
	std::vector<Obj3d*> objects;
	string add_functions_s;
	string add_uniform_s;

	template <class Head>
	Shader& append(Head* obj) {
		objects.push_back(obj);
		has_changed = true;
		return *this;
	}

	template <class Head, class ...Tail>
	Shader& append(Head* obj, Tail * ... args) {
		objects.push_back(obj);
		return append(args...);
	}

	string get_color_assemble(string const& body) {
		string start = "vec3 get_color(vec3 p){\n"
			"    float dist, min_dist = 1000000.;\n"
			"    vec3 color;\n";
		string end = "    return color;\n}\n";
		return start + body + end;
	}
	string get_dist_assemble(string const& body) {
		string start = "float get_dist(vec3 p){\n"
			"    float dist, min_dist = 1000000.;\n";
		string end = "    return min_dist;\n}";
		return start + body + end;
	}

	string assemble_shader() {
		string uniforms;
		string funcs;
		string get_color_body;
		string get_dist_body;
		std::set<string> used_types;
		for (Obj3d* obj : objects) {
			uniforms.append(obj->compose_uniforms_code());

			if (used_types.find(obj->type) == used_types.end()) {
				funcs.append(obj->code).append("\n");
				used_types.insert(obj->type);
			}

			get_dist_body.append(obj->compose_get_dist())
				.append("    if (dist < min_dist)\n"
					"        min_dist = dist;\n");
			get_color_body.append(obj->compose_get_dist())
				.append("    if (dist < min_dist){\n"
					"        min_dist = dist;\n"
					"        color = " + obj->color_func + ";\n"
					"    }\n");
		}
		return default_shader.body + add_uniform_s + add_functions_s + uniforms + funcs + get_color_assemble(get_color_body) + get_dist_assemble(get_dist_body);
	}

	sf::Shader& compile() {
		if (has_changed) {
			string some = assemble_shader();
			print(some);
			compiled_shader.loadFromMemory(some, sf::Shader::Fragment);
			has_changed = false;
		}
		return compiled_shader;
	}

	void send_uniforms() {
		for (Obj3d* obj : objects)
			obj->send_uniforms(compiled_shader);
	}

	Shader& add_func(string const& s) {
		add_functions_s.append(s);
		has_changed = true;
		return *this;
	}
	Shader& add_uniform(string const& s) {
		add_uniform_s.append(s);
		has_changed = true;
		return *this;
	}
};

string get_rnd_f() {
	return  "float rnd_(vec2 x){\n"\
		"    int n = int(x.x * 40.0 + x.y * 6400.0);\n"\
		"    n = (n << 13) ^ n;\n"\
		"    return 1.0 - float( (n * (n * n * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0;\n"\
		"}\n"\
		"vec2 rnd(vec2 x){\n"\
		"    return vec2(rnd_(x), rnd_(-x));\n"\
		"}\n";
}

string get_voronoy_f() {
	return  "vec3 col_grid(vec2 uv){\n"\
		"    float t = iTime;\n"\
		"    \n"\
		"    vec2 gv = fract(uv) - .5;\n"\
		"    vec2 id = floor(uv);\n"\
		"    float min_dist = 100.;\n"\
		"    for (int x = -1; x <= 1; x++){\n"\
		"        for (int y = -1; y <= 1; y++){\n"\
		"            vec2 offset = vec2(x, y);\n"\
		"            vec2 r = rnd(id+offset);\n"\
		"            vec2 pos = offset + cos(r*4729)*.5;\n"\
		"            \n"\
		"            float d = length(gv-pos);\n"\
		"            if (d < min_dist)\n"\
		"                min_dist = d;\n"\
		"        }\n"\
		"    }\n"\
		"    return vec3(min_dist);"\
		"}\n";
}