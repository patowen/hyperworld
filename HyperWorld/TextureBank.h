#pragma once

#include <unordered_map>
#include <memory>
#include "Texture.h"

enum class TextureHandle {TEST};

class TextureBank {
public:
	TextureBank() {
		textures[TextureHandle::TEST] = std::make_unique<Texture>();
	}

private:
	std::unordered_map<TextureHandle, std::unique_ptr<Texture>> textures;
};
