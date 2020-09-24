/*
	Copyright 2020 Patrick Owen

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
 */

#pragma once

#include "glad.h"
#include <vector>

#include "TextureData.h"

class Texture {
public:
	Texture(const TextureData& data, GLint wrap) {
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB8, data.width, data.height, 0, GL_RGB, GL_UNSIGNED_BYTE, data.data.data());
		glGenerateMipmap(GL_TEXTURE_2D);
	}

	~Texture() {
		glDeleteTextures(1, &texture);
	}

	Texture(const Texture&) = delete;
	Texture& operator=(const Texture&) = delete;

	void bind() {
		glBindTexture(GL_TEXTURE_2D, texture);
	}

private:
	GLuint texture;
};
