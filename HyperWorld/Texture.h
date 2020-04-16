#pragma once

#include <glad/glad.h>
#include <vector>

class Texture {
public:
	Texture() {
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);

		std::vector<GLubyte> textureData;
		for (int i=0; i<128*128; ++i) {
			textureData.push_back(255);
			textureData.push_back(255);
			textureData.push_back(0);
		}

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 128, 128, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData.data());
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