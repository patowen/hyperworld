#pragma once

#include "glad.h"
#include <array>
#include <vector>

#include "ShaderInterface.h"
#include "VectorMath.h"

class Vertex {
public:
	Vertex(std::array<float, 4> pos, std::array<float, 4> normal, std::array<float, 2> texCoord): pos(pos), normal(normal), texCoord(texCoord) {}
	std::array<float, 4> pos;
	std::array<float, 4> normal;
	std::array<float, 2> texCoord;
};

class Model {
public:
	Model(): buffers{0, 0}, vertexArray(0), numVertices(0), numElements(0) {}

	Model(const ShaderInterface& shaderInterface, const std::vector<Vertex>& vertices, const std::vector<GLuint>& elements) {
		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);

		glGenBuffers(2, buffers.data());

		glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
		numVertices = static_cast<GLsizei>(vertices.size());
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * numVertices, vertices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(shaderInterface.vPosLocation);
		glVertexAttribPointer(shaderInterface.vPosLocation, 4, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));
		glEnableVertexAttribArray(shaderInterface.vNormalLocation);
		glVertexAttribPointer(shaderInterface.vNormalLocation, 4, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
		glEnableVertexAttribArray(shaderInterface.vTexCoordLocation);
		glVertexAttribPointer(shaderInterface.vTexCoordLocation, 2, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texCoord)));

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
		numElements = static_cast<GLsizei>(elements.size());
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(GLuint) * elements.size(), elements.data(), GL_STATIC_DRAW);

		glBindVertexArray(0);
	}

	~Model() {
		glDeleteBuffers(2, buffers.data());
		glDeleteVertexArrays(1, &vertexArray);
	}

	Model(const Model&) = delete;
	Model& operator=(const Model&) = delete;

	Model(Model&& other) noexcept :
			buffers(other.buffers),
			vertexArray(other.vertexArray),
			numVertices(other.numVertices),
			numElements(other.numElements) {
		other.buffers.fill(0);
		other.vertexArray = 0;
	}

	Model&& operator=(Model&& other) noexcept {
		buffers = other.buffers;
		vertexArray = other.vertexArray;
		numVertices = other.numVertices;
		numElements = other.numElements;

		other.buffers.fill(0);
		other.vertexArray = 0;
	}

	void render() {
		glBindVertexArray(vertexArray);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffers[1]);
		glDrawElements(GL_TRIANGLES, numElements, GL_UNSIGNED_INT, 0);
	}

private:
	std::array<GLuint, 2> buffers; // vertexBuffer, elementBuffer
	GLuint vertexArray;
	GLsizei numVertices, numElements;
	// TODO: Texture, shader, primitive
};
