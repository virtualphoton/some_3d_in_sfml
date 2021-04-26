#include <SFML/Graphics.hpp>
#include <iostream>
#include "camera.h"
#include "vectors.h"
#include "default_shader.h"
#include <string>
#include "shader.h"
#include "objects3d.h"
#include "uniforms.h"
#include "useful_funcs.h"
#include "levels/lvl1.h"

const int W = 1024;
const int H = 768;

using std::vector;
using std::string;
int main() {
	Level1 gm(W, H);
	gm.run();
}