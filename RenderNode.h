#pragma once

class RenderContext;

class RenderNode {
public:
	virtual void render(RenderContext& context) = 0;
};
