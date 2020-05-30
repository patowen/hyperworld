#pragma once
#include <unordered_map>
#include "Model.h"
#include "ModelBuilder.h"
#include "Tessellation.h"

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
				Vector4d(0, 0, 1, -1),
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

Model makePlane(const ShaderInterface& shaderInterface) {
	ModelBuilder builder;
	Tessellation tessellation;
	tessellation.testTessellation();

	std::array<Vector2d, tessellation.n> texCoords { Vector2d(0, 0), Vector2d(1, 0), Vector2d(0, 1) };
	Vector4d normal(0, 0, 1, 0);

	for (size_t i=0; i<tessellation.getNumFaces(); ++i) {
		std::array<GLuint, tessellation.n> vertices;

		for (size_t j=0; j<tessellation.n; ++j) {
			vertices[j] = builder.addVertex(tessellation.getVertexPos(i, j), normal, texCoords[j]);
		}

		int orientation = tessellation.getOrientation(i);
		builder.addTriangle(vertices[0], vertices[(orientation + tessellation.n) % tessellation.n], vertices[(orientation * 2 + tessellation.n) % tessellation.n]);
	}

	return builder.build(shaderInterface);
}

Model makePrism(const ShaderInterface& shaderInterface) {
	ModelBuilder builder;
	builder.addPrism(Matrix4d::Identity(), 8, 1, 2, 60);

	return builder.build(shaderInterface);
}

enum class ModelHandle {DODECAHEDRON, HOROSPHERE, PLANE, PRISM};

class ModelBank {
public:
	ModelBank(const ShaderInterface& shaderInterface) {
		models[ModelHandle::DODECAHEDRON] = std::make_unique<Model>(makeDodecahedron(shaderInterface));
		models[ModelHandle::HOROSPHERE] = std::make_unique<Model>(makeHorosphere(shaderInterface));
		models[ModelHandle::PLANE] = std::make_unique<Model>(makePlane(shaderInterface));
		models[ModelHandle::PRISM] = std::make_unique<Model>(makePrism(shaderInterface));
	}

	void render(ModelHandle model) {
		models.at(model)->render();
	}

private:
	std::unordered_map<ModelHandle, std::unique_ptr<Model>> models;
};
