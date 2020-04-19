#pragma once

#include "glad.h"
#include <vector>

#include "TextureData.h"

namespace TextureLoader {
	TextureData loadTexture(std::string name);
}
