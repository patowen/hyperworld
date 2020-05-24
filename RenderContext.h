#pragma once
#include <unordered_map>
#include <memory>
#include <iostream>
#include "VectorMath.h"
#include "ShaderInterface.h"
#include "ModelBank.h"

class Model;

class RenderContext {
public:
	RenderContext(ShaderInterface &shaderInterface, ModelBank &modelBank, TextureBank &textureBank)
		: projection(Matrix4d::Identity()), modelView(Matrix4d::Identity()), shaderInterface(shaderInterface), modelBank(modelBank), textureBank(textureBank) {}

	void setProjection(const Matrix4d &projection) {
		this->projection = projection;
	}

	void resetModelView() {
		modelView = Matrix4d::Identity();
	}

	void setModelView(const Matrix4d &modelView) {
		this->modelView = modelView;
	}

	const Matrix4d &getModelView() const {
		return this->modelView;
	}

	void addModelView(const Matrix4d &modelView) {
		this->modelView *= modelView;
	}

	ModelBank &getModelBank() const {
		return modelBank;
	}

	ShaderInterface &getShaderInterface() const {
		return shaderInterface;
	}

	void useShader() {
		shaderInterface.use();
	}

	void setUniforms() {
		shaderInterface.setProjection(projection.cast<float>());
		shaderInterface.setModelView(modelView.cast<float>());
		shaderInterface.setLightPos(Vector4d(0, 0, 0, 1).cast<float>());
	}

	void setTexture(TextureHandle texture) {
		textureBank.bind(texture);
	}

	void render(ModelHandle model) {
		modelBank.render(model);
	}

private:
	Matrix4d projection;
	Matrix4d modelView;
	ShaderInterface &shaderInterface;
	ModelBank &modelBank;
	TextureBank &textureBank;
};
