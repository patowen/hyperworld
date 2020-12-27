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

	builder.addBackFaces();

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

	builder.addBackFaces();

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

	builder.addBackFaces();

	return builder.build(shaderInterface);
}

Model makePrism(const ShaderInterface& shaderInterface) {
	ModelBuilder builder;
	Matrix4d transform;
	transform << 1, 0, 0, 0,  0, -1, 0, 0,  0, 0, -1, 0,  0, 0, 0, 1;
	builder.addPrism(transform, 8, 1, 2, 60);

	builder.addBackFaces();

	return builder.build(shaderInterface);
}

class TreeBuilder {
public:
	TreeBuilder(): sideLength(acosh(3)) {
		Matrix4d translation = VectorMath::hyperbolicDisplacement(Vector4d(0, 0, sideLength, 0));

		recursiveTransformations.push_back(translation);
		recursiveTransformations.push_back(translation * VectorMath::rotation(Vector3d(1, 0, 0), M_TAU / 4.0));
		recursiveTransformations.push_back(translation * VectorMath::rotation(Vector3d(0, 1, 0), M_TAU / 4.0));
		recursiveTransformations.push_back(translation * VectorMath::rotation(Vector3d(-1, 0, 0), M_TAU / 4.0));
		recursiveTransformations.push_back(translation * VectorMath::rotation(Vector3d(0, -1, 0), M_TAU / 4.0));
	}

	void buildTree(ModelBuilder& builder, Matrix4d transform, int layers) {
		buildBranch(builder, transform, layers);
		buildBranch(builder, transform * VectorMath::rotation(Vector3d(1, 0, 0), M_TAU / 4.0), layers);
		buildBranch(builder, transform * VectorMath::rotation(Vector3d(0, 1, 0), M_TAU / 4.0), layers);
		buildBranch(builder, transform * VectorMath::rotation(Vector3d(-1, 0, 0), M_TAU / 4.0), layers);
		buildBranch(builder, transform * VectorMath::rotation(Vector3d(0, -1, 0), M_TAU / 4.0), layers);
		buildBranch(builder, transform * VectorMath::rotation(Vector3d(1, 0, 0), M_TAU / 2.0), layers);
	}

	void buildBranch(ModelBuilder& builder, Matrix4d transform, int layers) {
		if (transform(3, 3) > 100) {
			return;
		}

		builder.addPrism(transform, 8, 0.1, sideLength, 6);

		if (layers != 0) {
			for (int i = 0; i < recursiveTransformations.size(); ++i) {
				buildBranch(builder, transform * recursiveTransformations[i], layers - 1);
			}
		}
	}

private:
	std::vector<Matrix4d> recursiveTransformations;
	double sideLength;
};

Model makeTree(const ShaderInterface& shaderInterface) {
	ModelBuilder builder;

	TreeBuilder().buildTree(builder, Matrix4d::Identity(), 7);

	return builder.build(shaderInterface);
}

enum class ModelHandle {DODECAHEDRON, HOROSPHERE, PLANE, PRISM, TREE};

class ModelBank {
public:
	ModelBank(const ShaderInterface& shaderInterface) {
		models[ModelHandle::DODECAHEDRON] = std::make_unique<Model>(makeDodecahedron(shaderInterface));
		models[ModelHandle::HOROSPHERE] = std::make_unique<Model>(makeHorosphere(shaderInterface));
		models[ModelHandle::PLANE] = std::make_unique<Model>(makePlane(shaderInterface));
		models[ModelHandle::PRISM] = std::make_unique<Model>(makePrism(shaderInterface));
		models[ModelHandle::TREE] = std::make_unique<Model>(makeTree(shaderInterface));
	}

	void render(ModelHandle model) {
		models.at(model)->render();
	}

private:
	std::unordered_map<ModelHandle, std::unique_ptr<Model>> models;
};
