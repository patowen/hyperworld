#pragma once
#include "VectorMath.h"

class RenderContext;

class RenderNode {
public:
	virtual void render(RenderContext& context) = 0;
};
