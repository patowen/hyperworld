#pragma once
#include "RenderNode.h"
#include "RenderContext.h"
#include "VectorMath.h"
#include "ModelBank.h"
#include "TextureBank.h"

class SimpleRenderNode : public RenderNode {
public:
	SimpleRenderNode(Matrix4d transform, ModelHandle model, TextureHandle texture): transform(transform), model(model), texture(texture) {}

	void render(RenderContext& context) override {
		Matrix4d oldModelView = context.getModelView();
		context.addModelView(transform);
		context.setTexture(texture);
		context.render(model);
		context.setModelView(oldModelView);
	}

private:
	Matrix4d transform = Matrix4d::Identity();
	ModelHandle model = ModelHandle::DODECAHEDRON;
	TextureHandle texture = TextureHandle::BLANK;
};