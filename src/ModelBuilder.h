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

#include <vector>
#include "glad.h"
#include "VectorMath.h"
#include "Model.h"

using std::vector;
using std::array;

class ModelBuilder {
public:
	ModelBuilder(): currentVertex(0) {}

	Model build() {
		return Model(vertices, elements);
	}

	// Primitive functions
	GLuint addVertex(Vector4d pos, Vector4d normal, Vector2d texCoord) {
		vertices.emplace_back(
			std::array<float, 4>{static_cast<float>(pos[0]), static_cast<float>(pos[1]), static_cast<float>(pos[2]), static_cast<float>(pos[3])},
			std::array<float, 4>{static_cast<float>(normal[0]), static_cast<float>(normal[1]), static_cast<float>(normal[2]), static_cast<float>(normal[3])},
			std::array<float, 2>{static_cast<float>(texCoord[0]), static_cast<float>(texCoord[1])});

		return currentVertex++;
	}

	void addTriangle(GLuint vertex0, GLuint vertex1, GLuint vertex2) {
		elements.push_back(vertex0);
		elements.push_back(vertex1);
		elements.push_back(vertex2);
	}

	// Enhanced functions
	void addPolygonFace(std::vector<Vector4d> positions) {
		auto n = positions.size();

		Vector4d normal = VectorMath::hyperbolicNormal(positions[0], positions[n / 3], positions[(n * 2) / 3]);

		std::vector<GLuint> vertices;
		for (int i = 0; i < n; ++i) {
			vertices.push_back(addVertex(positions[i], normal, Vector2d(0.5 + 0.5 * cos(i * M_TAU / n), 0.5 + 0.5 * sin(i * M_TAU / n))));
		}

		for (decltype(n) i = 1; i < n - 1; ++i) {
			addTriangle(vertices[0], vertices[i], vertices[i+1]);
		}
	}

	void addPrism(const Matrix4d& transform, int sides, double radius, double length, int steps) {
		// Add vertices
		double sinhRadius = sinh(radius);
		double coshRadius = cosh(radius);

		vector<vector<array<GLuint, 2>>> prism;
		for (int side = 0; side < sides; ++side) {
			prism.emplace_back();
			auto& prismSide = prism.back();

			double angle0 = side * M_TAU / sides;
			double angle1 = ((side + 1) % sides) * M_TAU / sides;
			array<Vector4d, 2> basePos {
				Vector4d(cos(angle0) * sinh(radius), sin(angle0) * sinh(radius), 0, cosh(radius)),
				Vector4d(cos(angle1) * sinh(radius), sin(angle1) * sinh(radius), 0, cosh(radius))
			};
			Vector4d baseNormal = VectorMath::hyperbolicNormal(basePos[0], basePos[1], VectorMath::translation(basePos[0]) * Vector4d(0, 0, 1, sqrt(2)));
			baseNormal /= sqrt(VectorMath::hyperbolicSqrNorm(baseNormal));

			for (int step = 0; step <= steps; ++step) {
				prismSide.emplace_back();
				auto& prismSection = prismSide.back();
				Matrix4d stepTransform = transform * VectorMath::hyperbolicDisplacement(Vector4d(0, 0, length * step / steps, 0));
				for (int k=0; k<2; k++) {
					prismSection[k] = addVertex(stepTransform * basePos[k], stepTransform * baseNormal, Vector2d((double)step / steps, (double)(side + k) / sides));
				}
			}
		}

		// Add faces
		for (int side = 0; side < sides; ++side) {
			for (int step = 0; step < steps; ++step) {
				addTriangle(prism[side][step][0], prism[side][step][1], prism[side][step + 1][1]);
				addTriangle(prism[side][step][0], prism[side][step + 1][1], prism[side][step + 1][0]);
			}
		}
	}

	void addBackFaces() {
		int numExistingVertices = vertices.size();
		for (int i = 0; i < numExistingVertices; ++i) {
			const Vertex& existingVertex = vertices[i];
			const std::array<float, 4>& n = existingVertex.normal;
			vertices.emplace_back(existingVertex.pos, std::array<float, 4> { -n[0], -n[1], -n[2], -n[3] }, existingVertex.texCoord);
		}

		int numExistingElements = elements.size();
		for (int i = 0; i < numExistingElements; i += 3) {
			addTriangle(elements[i] + numExistingVertices, elements[i+2] + numExistingVertices, elements[i+1] + numExistingVertices);
		}
	}

private:
	GLuint currentVertex;
	std::vector<Vertex> vertices;
	std::vector<GLuint> elements;
};
