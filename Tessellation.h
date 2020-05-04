#pragma once
#include <array>
#include <limits>
#include <iostream>
#include "VectorMath.h"

class Tessellation {
public:
	void testTessellation() {
		createSeedFace();
		for (unsigned i=0; i<5; ++i) {
			std::cout << faces.size() << ", " << vertices.size() << "\n";
			size_t currentCount = faces.size();
			for (size_t j=0; j<currentCount; ++j) {
				for (unsigned k=0; k<n; ++k) {
					if (faces[j].adjacentFaces[k].index == FaceRef::nullRef()) {
						createAdjacentFace(FaceRef(j), k);
					}
				}
			}
		}

		std::cout << faces.size() << ", " << vertices.size() << "\n";
	}

private:
	static constexpr unsigned n = 3;
	std::array<unsigned, n> shape = {2, 2, 2}; // {2, 4, 5}

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

		std::array<FaceRef, n> adjacentFaces;
		std::array<VertexRef, n> adjacentVertices;
	};

	class Vertex {
	public:
		Vertex(unsigned type, unsigned numAdjacentFaces, FaceRef startFace):
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

		bool isSaturated() {
			return freeSlotMin == freeSlotMax;
		}

		FaceRef getUpperFace() {
			return adjacentFaces[freeSlotMax];
		}

		FaceRef getLowerFace() {
			return adjacentFaces[(freeSlotMin + adjacentFaces.size() - 1) % adjacentFaces.size()];
		}

		int type;
		std::vector<FaceRef> adjacentFaces;
		
		unsigned freeSlotMin;
		unsigned freeSlotMax;
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

	FaceRef createSeedFace() {
		faces.emplace_back();
		FaceRef newFace(faces.size());

		for (unsigned newVertexIndex = 0; newVertexIndex != n; ++newVertexIndex) {
			vertices.emplace_back(newVertexIndex, shape[newVertexIndex] * 2, newFace);
			VertexRef newVertex(vertices.size());
			getFace(newFace).adjacentVertices[newVertexIndex] = newVertex;
		}

		return newFace;
	}

	FaceRef createAdjacentFace(FaceRef face, unsigned edge) {
		unsigned lowerVertexIndex = (edge + 1u) % n;
		unsigned upperVertexIndex = (edge + 2u) % n;

		faces.emplace_back();
		FaceRef newFace(faces.size());

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

		// Vertex saturation prep
		unsigned descendingVertexIndex = lowerVertexIndex;
		unsigned ascendingVertexIndex = upperVertexIndex;
		VertexRef descendingVertex = lowerVertex;
		VertexRef ascendingVertex = upperVertex;
		unsigned descendingEdge = edge;
		unsigned ascendingEdge = edge;

		// Lower vertex saturation
		while (descendingVertexIndex != ascendingVertexIndex) {
			if (!getVertex(descendingVertex).isSaturated()) {
				break;
			}
			FaceRef existingFace = getVertex(descendingVertex).getLowerFace();
			descendingEdge = (descendingEdge + n - 1u) % n;
			descendingVertexIndex = (descendingVertexIndex + n - 1u) % n;
			descendingVertex = getFace(existingFace).adjacentVertices[descendingVertexIndex];

			getFace(newFace).adjacentFaces[descendingEdge] = existingFace;
			getFace(newFace).adjacentVertices[descendingVertexIndex] = descendingVertex;
			getVertex(descendingVertex).addUpperFace(newFace);
		}

		// Upper vertex saturation
		while (ascendingVertexIndex != descendingVertexIndex) {
			if (!getVertex(ascendingVertex).isSaturated()) {
				break;
			}
			FaceRef existingFace = getVertex(ascendingVertex).getUpperFace();
			ascendingEdge = (ascendingEdge + 1u) % n;
			ascendingVertexIndex = (ascendingVertexIndex + 1u) % n;
			ascendingVertex = getFace(existingFace).adjacentVertices[ascendingVertexIndex];

			getFace(newFace).adjacentFaces[ascendingEdge] = existingFace;
			getFace(newFace).adjacentVertices[ascendingVertexIndex] = ascendingVertex;
			getVertex(ascendingVertex).addLowerFace(newFace);
		}

		// Freshly-created vertices
		if (ascendingVertexIndex != descendingVertexIndex) {
			for (unsigned newVertexIndex = ascendingVertexIndex + 1u; newVertexIndex != descendingVertexIndex; newVertexIndex = (newVertexIndex + 1u) % n) {
				vertices.emplace_back(newVertexIndex, shape[newVertexIndex] * 2, newFace);
				VertexRef newVertex(vertices.size());
				getFace(newFace).adjacentVertices[newVertexIndex] = newVertex;
			}
		}

		return newFace;
	}
};
