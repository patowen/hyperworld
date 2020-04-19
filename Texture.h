#pragma once

#include "glad.h"
#include <vector>

#include "TextureData.h"

class Texture {
public:
	Texture(const TextureData& data) {
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, data.width, data.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data.data());
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	~Texture() {
		glDeleteTextures(1, &texture);
	}

	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

private:
	GLuint texture;
};
