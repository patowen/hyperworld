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

#include <unordered_map>
#include <memory>
#include "Texture.h"
#include "TextureLoader.h"

enum class TextureHandle {PERLIN, TILE, BLANK};

class TextureBank {
public:
	TextureBank() {
		textures[TextureHandle::PERLIN] = std::make_unique<Texture>(TextureLoader::loadTexture("perlin.png"), GL_CLAMP_TO_EDGE);
		textures[TextureHandle::TILE] = std::make_unique<Texture>(TextureLoader::loadTexture("tile.png"), GL_REPEAT);
		textures[TextureHandle::BLANK] = std::make_unique<Texture>(makeBlankTexture(), GL_REPEAT);
	}

	void bind(TextureHandle textureHandle) {
		textures[textureHandle]->bind();
	}

private:
	std::unordered_map<TextureHandle, std::unique_ptr<Texture>> textures;

	TextureData makeBlankTexture() {
		TextureData data(1, 1);
		data.data.push_back(255);
		data.data.push_back(255);
		data.data.push_back(255);
		return data;
	}
};
