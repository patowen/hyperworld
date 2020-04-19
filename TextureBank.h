#pragma once

#include <unordered_map>
#include <memory>
#include "Texture.h"
#include "TextureLoader.h"

enum class TextureHandle {PERLIN};

class TextureBank {
public:
	TextureBank() {
		TextureData data = TextureLoader::loadTexture("perlin.png");
		textures[TextureHandle::PERLIN] = std::make_unique<Texture>(data);
	}

	void bind(TextureHandle textureHandle) {
		textures[textureHandle]->bind();
	}

private:
	std::unordered_map<TextureHandle, std::unique_ptr<Texture>> textures;
};
