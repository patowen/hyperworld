/*
	Copyright 2020 Patrick Owen

	Licensed under the Apache License, Version 2.0 (the "License");
	you may not use this file except in compliance with the License.
	You may obtain a copy of the License at

		http://www.apache.org/licenses/LICENSE-2.0

	Unless required by applicable law or agreed to in writing, software
	distributed under the License is distributed on an "AS IS" BASIS,
	WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
	See the License for the specific language governing permissions and
	limitations under the License.
 */

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