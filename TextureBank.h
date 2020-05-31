#pragma once

#include <unordered_map>
#include <memory>
#include "Texture.h"
#include "TextureLoader.h"

enum class TextureHandle {PERLIN, TILE, BLANK};

class TextureBank {
public:
	TextureBank() {
		textures[TextureHandle::PERLIN] = std::make_unique<Texture>(TextureLoader::loadTexture("perlin.png"));
		textures[TextureHandle::TILE] = std::make_unique<Texture>(TextureLoader::loadTexture("tile.png"));
		textures[TextureHandle::BLANK] = std::make_unique<Texture>(makeBlankTexture());
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
