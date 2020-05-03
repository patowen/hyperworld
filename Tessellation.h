#pragma once
#include <array>
#include <limits>
#include "VectorMath.h"

class Tessellation {
private:
	std::array<unsigned, 3> shape = {2, 4, 5};

	class Face;
	class Vertex;
	std::vector<Face> faces;
	std::vector<Vertex> vertices;

	class FaceRef {
	public:
		static constexpr size_t nullRef() {
			return std::numeric_limits<size_t>::max();
		}

		FaceRef(): index(nullRef()) {}
		FaceRef(size_t index): index(index) {}
		size_t index;
	};

	class VertexRef {
	public:
		static constexpr size_t nullRef() {
			return std::numeric_limits<size_t>::max();
		}

		VertexRef(): index(nullRef()) {}
		VertexRef(size_t index): index(index) {}
		size_t index;
	};

	class Face {
	public:
		Face() {
			adjacentFaces.fill(nullIndex());
		}

		std::array<FaceRef, 3> adjacentFaces;
		std::array<VertexRef, 3> adjacentVertices;
	};

	class Vertex {
	public:
		Vertex(unsigned type, unsigned numAdjacentFaces, size_t startFace):
			type(type),
			adjacentFaces(numAdjacentFaces, nullIndex()),
			freeSlotMin(0),
			freeSlotMax(numAdjacentFaces - 1) {
			adjacentFaces[numAdjacentFaces - 1] = startFace;
		}

		void addUpperFace(FaceRef face) {
			adjacentFaces[freeSlotMax - 1] = face;
			freeSlotMax--;
		}

		void addLowerFace(FaceRef face) {
			adjacentFaces[freeSlotMin] = face;
			freeSlotMin++;
		}

		int type;
		std::vector<FaceRef> adjacentFaces;
		
		int freeSlotMin;
		int freeSlotMax;
	};

	Face& getFace(FaceRef face) {
		return faces[face.index];
	}

	Vertex& getVertex(VertexRef vertex) {
		return vertices[vertex.index];
	}

	static constexpr size_t nullIndex() {
		return std::numeric_limits<size_t>::max();
	}

	size_t createAdjacentFace(FaceRef face, unsigned edge) {
		unsigned lowerVertexIndex = (edge + 1u) % 3u;
		unsigned upperVertexIndex = (edge + 2u) % 3u;

		faces.emplace_back();
		FaceRef newFace = faces.size();

		// Sort out all direct adjacencies
		// Face to face
		getFace(face).adjacentFaces[edge] = newFace;
		getFace(newFace).adjacentFaces[edge] = face;

		// Face to vertex
		VertexRef lowerVertex = getFace(face).adjacentVertices[lowerVertexIndex];
		VertexRef upperVertex = getFace(face).adjacentVertices[upperVertexIndex];
		getFace(newFace).adjacentVertices[lowerVertexIndex] = lowerVertex;
		getFace(newFace).adjacentVertices[upperVertexIndex] = upperVertex;

		// Vertex to face
		getVertex(lowerVertex).addUpperFace(newFace);
		getVertex(upperVertex).addLowerFace(newFace);

		// TODO: Check for saturation and add extra adjacencies if there
	}
};
