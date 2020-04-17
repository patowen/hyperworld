#pragma once

#include <unordered_map>
#include <memory>
#include "Texture.h"
#include "TextureLoader.h"

enum class TextureHandle {TEST};

class TextureBank {
public:
	TextureBank() {
		TextureData data = TextureLoader::loadTexture("test.png");
		textures[TextureHandle::TEST] = std::make_unique<Texture>(data);
	}

private:
	std::unordered_map<TextureHandle, std::unique_ptr<Texture>> textures;
};
