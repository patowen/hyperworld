#pragma once

#include <glad/glad.h>
#include <Eigen/Dense>
#include <array>
#include <vector>

#include "ShaderInterface.h"

class Vertex {
public:
	Vertex(float x, float y, float r, float g, float b): pos{x, y}, col{r, g, b} {}
	std::array<float, 2> pos;
	std::array<float, 3> col;
};

class Model {
public:
	Model(const ShaderInterface &shaderInterface, const std::vector<Vertex> &vertices): shaderInterface(shaderInterface) {
		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);

		glGenBuffers(1, &vertexBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer);
		numVertices = static_cast<GLsizei>(vertices.size());
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * numVertices, vertices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(shaderInterface.vposLocation);
		glVertexAttribPointer(shaderInterface.vposLocation, 2, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));
		glEnableVertexAttribArray(shaderInterface.vcolLocation);
		glVertexAttribPointer(shaderInterface.vcolLocation, 3, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, col)));

		glBindVertexArray(0);
	}

	~Model() {
		glDeleteBuffers(1, &vertexBuffer);
		glDeleteVertexArrays(1, &vertexArray);
	}

	Model(const Model&) = delete;
	Model& operator=(const Model&) = delete;

	void render() {
		glBindVertexArray(vertexArray);
		glDrawArrays(GL_TRIANGLES, 0, numVertices);
	}

private:
	const ShaderInterface &shaderInterface;
	GLuint vertexBuffer;
	GLuint vertexArray;
	GLsizei numVertices;
	// TODO: Texture, shader, primitive
};