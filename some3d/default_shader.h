#pragma once
#include <string>
#include <fstream>

using std::string;
/* reads text of base shader(which will be used for creating other shaders) and writes it into body field of the class
* fields:
*	string body - code of shader
*/
class DefaultShader {
public:
	string body;

	DefaultShader() {
		std::fstream newfile;
		newfile.open("shaders/dynamic_shader_base.frag", std::ios::in);
		string tp;
		while (getline(newfile, tp))
			body.append(tp).append("\n");
		newfile.close();
	}
} default_shader;