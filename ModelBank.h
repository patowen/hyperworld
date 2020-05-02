#pragma once
#include <unordered_map>
#include "Model.h"
#include "ModelBuilder.h"

Model makeHelloWorldTriangle(const ShaderInterface& shaderInterface) {
	std::vector<Vertex> vertices {
		Vertex({-0.693f, -0.4f, 0.0f, 1.0f}, {1.f, 0.f, 0.f, 1.0f}, {0.0f, 0.0f}),
		Vertex({0.693f, -0.4f, 0.0f, 1.0f}, {0.f, 1.f, 0.f, 1.0f}, {1.0f, 0.0f}),
		Vertex({0.f, 0.8f, 0.0f, 1.0f}, {0.f, 0.f, 1.f, 1.0f}, {0.0f, 1.0f}),
	};

	return Model(shaderInterface, vertices, {0, 1, 2});
}

Model makeDodecahedron(const ShaderInterface& shaderInterface) {
	float s = 0.31546169558954995f;
	float goldenRatio = (1.0f + sqrtf(5.0f)) / 2.0f;
	float q = s / goldenRatio, p = s * goldenRatio;

	float factor = 2.0f / (1.0f - s * s * 3.0f);

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

	ModelBuilder builder;

	for (const auto& face : table) {
		std::vector<Vector4d> faceVertices;

		for (const auto& faceVertexData : face) {
			faceVertices.emplace_back(
				faceVertexData[0] * factor,
				faceVertexData[1] * factor,
				faceVertexData[2] * factor,
				factor - 1.0f
			);
		}

		builder.addPolygonFace(faceVertices);
	}

	return builder.build(shaderInterface);
}

Model makeHorosphere(const ShaderInterface& shaderInterface) {
	int numSteps = 400;
	double size = 20;
	double textureSize = 5;

	ModelBuilder builder;

	Eigen::Array<GLuint, Eigen::Dynamic, Eigen::Dynamic> vertices(numSteps + 1, numSteps + 1);

	for (int i=0; i<=numSteps; ++i) {
		for (int j=0; j<=numSteps; ++j) {
			double xPos = (static_cast<double>(i)/numSteps-0.5) * size;
			double yPos = (static_cast<double>(j)/numSteps-0.5) * size;
			Matrix4d rotation = VectorMath::horoRotation(xPos, yPos);
			vertices(i, j) = builder.addVertex(
				rotation * Vector4d(0, 0, 0, 1),
				rotation * Vector4d(0, 0, 1, 0),
				Vector2d(xPos * textureSize, yPos * textureSize));
		}
	}

	for (int i=0; i<numSteps; ++i) {
		for (int j=0; j<numSteps; ++j) {
			builder.addTriangle(vertices(i, j), vertices(i+1, j), vertices(i, j+1));
			builder.addTriangle(vertices(i, j+1), vertices(i+1, j), vertices(i+1, j+1));
		}
	}

	return builder.build(shaderInterface);
}

enum class ModelHandle {DODECAHEDRON, HOROSPHERE};

class ModelBank {
public:
	ModelBank(const ShaderInterface& shaderInterface) {
		models[ModelHandle::DODECAHEDRON] = std::make_unique<Model>(makeDodecahedron(shaderInterface));
		models[ModelHandle::HOROSPHERE] = std::make_unique<Model>(makeHorosphere(shaderInterface));
	}

	void render(ModelHandle model) {
		models.at(model)->render();
	}

private:
	std::unordered_map<ModelHandle, std::unique_ptr<Model>> models;
};
