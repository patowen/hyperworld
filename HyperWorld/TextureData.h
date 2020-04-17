#pragma once

#include <GLFW/glfw3.h>
#include <vector>

class TextureData {
public:
	int width;
	int height;
	std::vector<GLubyte> data;
};
