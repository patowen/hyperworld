#pragma once

#include <GLFW/glfw3.h>
#include <vector>

class TextureData {
public:
	TextureData(int width, int height): width(width), height(height) {}
	int width;
	int height;
	std::vector<GLubyte> data;
};
