#pragma once

#include <vector>
#include <glad/glad.h>
#include "VectorMath.h"
#include "Model.h"

class ModelBuilder {
public:
	ModelBuilder(): currentVertex(0) {}

	Model build(const ShaderInterface& shaderInterface) {
		return Model(shaderInterface, vertices, elements);
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

private:
	GLuint currentVertex;
	std::vector<Vertex> vertices;
	std::vector<GLuint> elements;
};
