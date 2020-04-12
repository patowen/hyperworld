#pragma once

#include <vector>
#include <glad/glad.h>
#include "VectorMath.h"

class ModelBuilder {
public:
	class VertexHandle {
	private:
		GLuint index;
	};

	// Primitive functions
	VertexHandle addVertex(Vector4d pos, Vector4d normal, Vector2d texture);
	void addTriangle(VertexHandle vertex0, VertexHandle vertex1, VertexHandle vertex2);

	// Enhanced functions (TODO)
};