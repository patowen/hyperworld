#pragma once

#include <vector>
#include <glad/glad.h>
#include "VectorMath.h"
#include "Model.h"

class ModelBuilder {
public:
	// Primitive functions
	GLuint addVertex(Vector4d pos, Vector4d normal, Vector2d texture);
	void addTriangle(GLuint vertex0, GLuint vertex1, GLuint vertex2);

	// Enhanced functions (TODO)
	void addPolygonFace(std::vector<Vector4d> positions) {
		auto n = static_cast<int>(positions.size());

		std::vector<GLuint> vertices;
		for (int i = 0; i < n; ++i) {
			vertices.push_back(addVertex(positions[i], Vector4d::Zero(), Vector2d(cos(i * M_TAU / n), sin(i * M_TAU / n))));
		}

		for (int i = 1; i < n - 1; ++i) {
			addTriangle(vertices[0], vertices[i], vertices[i+1]);
		}
	}
};
