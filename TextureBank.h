#pragma once

#include <unordered_map>
#include <memory>
#include "Texture.h"
#include "TextureLoader.h"

enum class TextureHandle {PERLIN, TILE};

class TextureBank {
public:
	TextureBank() {
		textures[TextureHandle::PERLIN] = std::make_unique<Texture>(TextureLoader::loadTexture("perlin.png"));
		textures[TextureHandle::TILE] = std::make_unique<Texture>(TextureLoader::loadTexture("tile.png"));
	}

	void bind(TextureHandle textureHandle) {
		textures[textureHandle]->bind();
	}

private:
	std::unordered_map<TextureHandle, std::unique_ptr<Texture>> textures;
};
