#pragma once
#include <string>
#include <fstream>

using std::string;

class DefaultShader {
public:
	string body;

	DefaultShader() {
		std::fstream newfile;
		newfile.open("shaders/dynamic_shader_base.frag", std::ios::in);
		string tp;
		string * field;
		while (getline(newfile, tp))
			body.append(tp).append("\n");
		newfile.close();
	}
} default_shader;