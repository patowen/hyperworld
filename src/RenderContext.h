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
#include <unordered_map>
#include <memory>
#include "VectorMath.h"
#include "ShaderProgramBank.h"
#include "ModelBank.h"

class Model;

class RenderContext {
public:
	RenderContext(ShaderProgramBank &shaderProgramBank, ModelBank &modelBank, TextureBank &textureBank) :
		shaderProgramBank(shaderProgramBank),
		modelBank(modelBank),
		textureBank(textureBank) {}

	RenderContext(const RenderContext&) = delete;
	RenderContext& operator=(const RenderContext&) = delete;

	RenderContext(RenderContext&& other) noexcept = default;
	RenderContext& operator=(RenderContext&& other) noexcept = default;

	void setDimensions(int width, int height) {
		this->width = width;
		this->height = height;
	}

	int getWidth() {
		return width;
	}

	int getHeight() {
		return height;
	}

	void setProjection(const Matrix4d &projection) {
		this->projection = projection;
		projectionInvalidated = true;
	}

	void resetModelView() {
		modelView = Matrix4d::Identity();
		modelViewInvalidated = true;
	}

	void setModelView(const Matrix4d &modelView) {
		this->modelView = modelView;
		modelViewInvalidated = true;
	}

	const Matrix4d &getModelView() const {
		return this->modelView;
	}

	void addModelView(const Matrix4d &modelView) {
		this->modelView *= modelView;
		modelViewInvalidated = true;
	}

	void setTexture(TextureHandle texture) {
		textureBank.bind(texture);
	}

	void setSpherical(bool spherical) {
		this->spherical = spherical;
		shaderProgramHandle = spherical ? ShaderProgramHandle::SPHERICAL : ShaderProgramHandle::HYPERBOLIC;
		shaderProgramInvalidated = true;
	}

	void render(ModelHandle model) {
		setUniforms();
		modelBank.render(model);
	}

private:
	Matrix4d projection = Matrix4d::Identity();
	Matrix4d modelView = Matrix4d::Identity();
	ShaderProgramBank& shaderProgramBank;
	ShaderProgramHandle shaderProgramHandle = ShaderProgramHandle::HYPERBOLIC;
	ModelBank& modelBank;
	TextureBank& textureBank;
	int width = 1;
	int height = 1;
	bool shaderProgramInvalidated = true;
	bool projectionInvalidated = true;
	bool modelViewInvalidated = true;
	bool spherical = false;

	void setUniforms() {
		ShaderProgram& shaderProgram = shaderProgramBank.get(shaderProgramHandle);

		if (shaderProgramInvalidated) {
			shaderProgram.use();
			projectionInvalidated = true;
			modelViewInvalidated = true;
		}

		if (projectionInvalidated) {
			shaderProgram.setProjection(projection.cast<float>());
			projectionInvalidated = false;
		}

		if (modelViewInvalidated) {
			shaderProgram.setModelView(modelView.cast<float>());
			shaderProgram.setLightPos((VectorMath::hyperbolicTranspose(modelView) * Vector4d(0, 0, 0, 1)).cast<float>());
			modelViewInvalidated = false;
		}
	}
};
