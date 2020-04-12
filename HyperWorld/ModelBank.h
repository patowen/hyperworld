#pragma once
#include <unordered_map>
#include "Model.h"

Model makeHelloWorldTriangle(const ShaderInterface& shaderInterface) {
	std::vector<Vertex> vertices {
		Vertex({-0.693f, -0.4f, 0.0f, 1.0f}, {1.f, 0.f, 0.f}),
		Vertex({0.693f, -0.4f, 0.0f, 1.0f}, {0.f, 1.f, 0.f}),
		Vertex({0.f, 0.8f, 0.0f, 1.0f}, {0.f, 0.f, 1.f}),
	};

	return Model(shaderInterface, vertices, {0, 1, 2});
}

Model makeDodecahedron(const ShaderInterface& shaderInterface) {
	float s = 0.31546169558954995f;
	float goldenRatio = (1.0f + sqrtf(5.0f)) / 2.0f;
	float q = s / goldenRatio, p = s * goldenRatio;

	std::array<std::array<std::array<float, 3>, 5>, 12> table = {{
		{{{ 0,  q,  p}, { s,  s,  s}, { q,  p,  0}, {-q,  p,  0}, {-s,  s,  s}}},
		{{{ 0,  q, -p}, {-s,  s, -s}, {-q,  p,  0}, { q,  p,  0}, { s,  s, -s}}},
		{{{ 0, -q,  p}, {-s, -s,  s}, {-q, -p,  0}, { q, -p,  0}, { s, -s,  s}}},
		{{{ 0, -q, -p}, { s, -s, -s}, { q, -p,  0}, {-q, -p,  0}, {-s, -s, -s}}},

		{{{ p,  0,  q}, { s,  s,  s}, { 0,  q,  p}, { 0, -q,  p}, { s, -s,  s}}},
		{{{-p,  0,  q}, {-s, -s,  s}, { 0, -q,  p}, { 0,  q,  p}, {-s,  s,  s}}},
		{{{ p,  0, -q}, { s, -s, -s}, { 0, -q, -p}, { 0,  q, -p}, { s,  s, -s}}},
		{{{-p,  0, -q}, {-s,  s, -s}, { 0,  q, -p}, { 0, -q, -p}, {-s, -s, -s}}},

		{{{ q,  p,  0}, { s,  s,  s}, { p,  0,  q}, { p,  0, -q}, { s,  s, -s}}},
		{{{ q, -p,  0}, { s, -s, -s}, { p,  0, -q}, { p,  0,  q}, { s, -s,  s}}},
		{{{-q,  p,  0}, {-s,  s, -s}, {-p,  0, -q}, {-p,  0,  q}, {-s,  s,  s}}},
		{{{-q, -p,  0}, {-s, -s,  s}, {-p,  0,  q}, {-p,  0, -q}, {-s, -s, -s}}},
	}};

	std::vector<Vertex> vertices;
	std::vector<GLuint> elements;

	int faceNum = 0;

	for (const auto& face : table) {
		std::array<float, 3> faceColor {faceNum / 12.0f, 1.0f, 0.0f};
		GLuint i = static_cast<GLuint>(vertices.size());
		for (const auto& faceVertex : face) {
			vertices.emplace_back(std::array<float, 4> {faceVertex[0], faceVertex[1], faceVertex[2], 1.0f}, faceColor);
		}

		std::array<GLuint, 9> nextElements {i, i+1, i+2, i, i+2, i+3, i, i+3, i+4};
		elements.insert(elements.end(), nextElements.cbegin(), nextElements.cend());

		++faceNum;
	}

	return Model(shaderInterface, vertices, elements);
}

enum class ModelHandle {TRIANGLE};

class ModelBank {
public:
	ModelBank(const ShaderInterface& shaderInterface) {
		models[ModelHandle::TRIANGLE] = std::make_unique<Model>(makeDodecahedron(shaderInterface));
	}

	void render(ModelHandle model) {
		models.at(model)->render();
	}

private:
	std::unordered_map<ModelHandle, std::unique_ptr<Model>> models;
};
