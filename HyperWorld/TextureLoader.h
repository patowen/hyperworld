#pragma once

#include <glad/glad.h>
#include <vector>

#include "TextureData.h"

namespace TextureLoader {
	TextureData loadTexture(std::string name);
}
