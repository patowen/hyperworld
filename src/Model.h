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

#include "glad.h"
#include <array>
#include <vector>

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

	Model(const std::vector<Vertex>& vertices, const std::vector<GLuint>& elements) {
		glGenVertexArrays(1, &vertexArray);
		glBindVertexArray(vertexArray);

		glGenBuffers(2, buffers.data());

		glBindBuffer(GL_ARRAY_BUFFER, buffers[0]);
		numVertices = static_cast<GLsizei>(vertices.size());
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * numVertices, vertices.data(), GL_STATIC_DRAW);

		glEnableVertexAttribArray(vPosLocation);
		glVertexAttribPointer(vPosLocation, 4, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, pos)));
		glEnableVertexAttribArray(vNormalLocation);
		glVertexAttribPointer(vNormalLocation, 4, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
		glEnableVertexAttribArray(vTexCoordLocation);
		glVertexAttribPointer(vTexCoordLocation, 2, GL_FLOAT, false, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, texCoord)));

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

	Model& operator=(Model&& other) noexcept {
		buffers = other.buffers;
		vertexArray = other.vertexArray;
		numVertices = other.numVertices;
		numElements = other.numElements;

		other.buffers.fill(0);
		other.vertexArray = 0;
		return *this;
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
	static const GLuint vPosLocation = 0, vNormalLocation = 1, vTexCoordLocation = 2;
	friend class ShaderInterface;
	// TODO: Potentially bundle with shader and primitive?
};
