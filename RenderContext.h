#pragma once
#include <unordered_map>
#include <memory>
#include "VectorMath.h"
#include "ShaderInterface.h"
#include "ModelBank.h"

class Model;

class RenderContext {
public:
	RenderContext(ShaderInterface &shaderInterface, ModelBank &modelBank, TextureBank &textureBank) :
		shaderInterface(shaderInterface),
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

	void useShader() {
		shaderInterface.use();
	}

	void setTexture(TextureHandle texture) {
		textureBank.bind(texture);
	}

	void render(ModelHandle model) {
		setUniforms();
		modelBank.render(model);
	}

private:
	Matrix4d projection = Matrix4d::Identity();
	Matrix4d modelView = Matrix4d::Identity();
	ShaderInterface& shaderInterface;
	ModelBank& modelBank;
	TextureBank& textureBank;
	int width = 1;
	int height = 1;
	bool projectionInvalidated = true;
	bool modelViewInvalidated = true;

	void setUniforms() {
		if (projectionInvalidated) {
			shaderInterface.setProjection(projection.cast<float>());
			projectionInvalidated = false;
		}

		if (modelViewInvalidated) {
			shaderInterface.setModelView(modelView.cast<float>());
			shaderInterface.setLightPos((VectorMath::isometricInverse(modelView) * Vector4d(0, 0, 0, 1)).cast<float>());
			modelViewInvalidated = false;
		}
	}
};
