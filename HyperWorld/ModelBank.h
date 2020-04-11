#pragma once
#include <unordered_map>
#include "Model.h"

enum class ModelHandle {TRIANGLE};

class ModelBank {
public:
	ModelBank(const ShaderInterface &shaderInterface) {
		std::vector<Vertex> vertices {Vertex(-0.693f, -0.4f, 1.f, 0.f, 0.f), Vertex(0.693f, -0.4f, 0.f, 1.f, 0.f), Vertex(0.f, 0.8f, 0.f, 0.f, 1.f)};
		models.emplace(ModelHandle::TRIANGLE, std::make_unique<Model>(shaderInterface, vertices));
	}

	void render(ModelHandle model) {
		models.at(model)->render();
	}

private:
	std::unordered_map<ModelHandle, std::unique_ptr<Model>> models;
};
