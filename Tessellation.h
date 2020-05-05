#pragma once
#include <array>
#include <limits>
#include <iostream>
#include "VectorMath.h"

class Tessellation {
public:
	void testTessellation() {
		createSeedFace();
		for (unsigned i=0; i<15; ++i) {
			std::cout << faces.size() << ", " << vertices.size() << "\n";
			size_t currentCount = faces.size();

			for (size_t j=0; j<currentCount; ++j) {
				for (unsigned k=0; k<n; ++k) {
					if (faces[j].adjacentFaces[k].index == FaceRef::nullRef()) {
						FaceRef face = createAdjacentFace(FaceRef(j), k);
					}
				}
			}
		}

		std::cout << faces.size() << ", " << vertices.size() << "\n";
		std::cout << "Done\n";
	}

private:
	static constexpr unsigned n = 3;
	std::array<unsigned, n> shape = {2, 3, 5}; // {2, 4, 5}

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

		bool isNull() {
			return index == nullRef();
		}
	};

	class VertexRef {
	public:
		static constexpr size_t nullRef() {
			return std::numeric_limits<size_t>::max();
		}

		VertexRef(): index(nullRef()) {}
		VertexRef(size_t index): index(index) {}
		size_t index;

		bool isNull() {
			return index == nullRef();
		}
	};

	class Face {
	public:
		Face(int orientation): orientation(orientation) {}
		int orientation;
		std::array<FaceRef, n> adjacentFaces;
		std::array<VertexRef, n> adjacentVertices;
	};

	class Vertex {
	public:
		Vertex(unsigned type, unsigned numAdjacentFaces, FaceRef startFace):
			type(type),
			adjacentFaces(numAdjacentFaces),
			freeSlotMin(0),
			freeSlotMax(numAdjacentFaces - 1) {
			adjacentFaces[numAdjacentFaces - 1] = startFace;
		}

		void addFace(FaceRef face, int direction) {
			if (direction == 1) {
				addUpperFace(face);
			} else {
				addLowerFace(face);
			}
		}

		bool isSaturated() {
			return freeSlotMin == freeSlotMax;
		}

		FaceRef getFace(int direction) {
			if (direction == 1) {
				return getUpperFace();
			} else {
				return getLowerFace();
			}
		}

		int type;
		std::vector<FaceRef> adjacentFaces;

		unsigned freeSlotMin;
		unsigned freeSlotMax;

	private:
		void addUpperFace(FaceRef face) {
			if (isSaturated()) {
				throw std::runtime_error("Tried to add to a saturated vertex");
			}

			adjacentFaces[freeSlotMax - 1] = face;
			freeSlotMax--;
		}

		void addLowerFace(FaceRef face) {
			if (isSaturated()) {
				throw std::runtime_error("Tried to add to a saturated vertex");
			}

			adjacentFaces[freeSlotMin] = face;
			freeSlotMin++;
		}

		FaceRef getUpperFace() {
			return adjacentFaces[freeSlotMax];
		}

		FaceRef getLowerFace() {
			return adjacentFaces[(freeSlotMin + adjacentFaces.size() - 1) % adjacentFaces.size()];
		}
	};

	Face& getFace(FaceRef face) {
		return faces[face.index];
	}

	Vertex& getVertex(VertexRef vertex) {
		return vertices[vertex.index];
	}

	void inspectFace(FaceRef faceRef) {
		Face& face = getFace(faceRef);

		std::cout << "Face " << faceRef.index << ":\n";
		std::cout << "  adjacent faces: ";
		for (FaceRef adj : face.adjacentFaces) {
			std::cout << (adj.isNull() ? "n" : std::to_string(adj.index)) << ", ";
		}
		std::cout << "\n  adjacent vertices: ";
		for (VertexRef adj : face.adjacentVertices) {
			if (adj.isNull()) {
				std::cout << "n, ";
			} else {
				std::cout << adj.index << "(" << (getVertex(adj).freeSlotMax - getVertex(adj).freeSlotMin) << "), ";
			}
		}
		std::cout << "\n";
	}

	FaceRef createSeedFace() {
		faces.emplace_back(1);
		FaceRef newFace(faces.size() - 1);

		for (unsigned newVertexIndex = 0; newVertexIndex != n; ++newVertexIndex) {
			vertices.emplace_back(newVertexIndex, shape[newVertexIndex] * 2, newFace);
			VertexRef newVertex(vertices.size() - 1);
			getFace(newFace).adjacentVertices[newVertexIndex] = newVertex;
		}

		return newFace;
	}

	FaceRef createAdjacentFace(FaceRef face, unsigned edge) {
		unsigned lowerVertexIndex = (edge + 1u) % n;
		unsigned upperVertexIndex = (edge + 2u) % n;

		int orientation = -getFace(face).orientation;

		if (orientation != 1 && orientation != -1) {
			throw std::runtime_error("Bad orientation");
		}

		faces.emplace_back(orientation);
		FaceRef newFace(faces.size() - 1);

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
		getVertex(lowerVertex).addFace(newFace, orientation);
		getVertex(upperVertex).addFace(newFace, -orientation);

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
			FaceRef existingFace = getVertex(descendingVertex).getFace(-orientation);
			descendingEdge = (descendingEdge + n - 1u) % n;
			descendingVertexIndex = (descendingVertexIndex + n - 1u) % n;
			descendingVertex = getFace(existingFace).adjacentVertices[descendingVertexIndex];

			getFace(newFace).adjacentFaces[descendingEdge] = existingFace;
			getFace(existingFace).adjacentFaces[descendingEdge] = newFace;
			getFace(newFace).adjacentVertices[descendingVertexIndex] = descendingVertex;
			if (descendingVertexIndex != ascendingVertexIndex) {
				getVertex(descendingVertex).addFace(newFace, orientation);
			}
		}

		// Upper vertex saturation
		while (ascendingVertexIndex != descendingVertexIndex) {
			if (!getVertex(ascendingVertex).isSaturated()) {
				break;
			}
			FaceRef existingFace = getVertex(ascendingVertex).getFace(orientation);
			ascendingEdge = (ascendingEdge + 1u) % n;
			ascendingVertexIndex = (ascendingVertexIndex + 1u) % n;
			ascendingVertex = getFace(existingFace).adjacentVertices[ascendingVertexIndex];

			getFace(newFace).adjacentFaces[ascendingEdge] = existingFace;
			getFace(existingFace).adjacentFaces[ascendingEdge] = newFace;
			getFace(newFace).adjacentVertices[ascendingVertexIndex] = ascendingVertex;
			if (ascendingVertexIndex != descendingVertexIndex) {
				getVertex(ascendingVertex).addFace(newFace, -orientation);
			}
		}

		// Freshly-created vertices
		if (ascendingVertexIndex != descendingVertexIndex) {
			for (unsigned newVertexIndex = (ascendingVertexIndex + 1u) % n; newVertexIndex != descendingVertexIndex; newVertexIndex = (newVertexIndex + 1u) % n) {
				vertices.emplace_back(newVertexIndex, shape[newVertexIndex] * 2, newFace);
				VertexRef newVertex(vertices.size() - 1);
				getFace(newFace).adjacentVertices[newVertexIndex] = newVertex;
			}
		}

		return newFace;
	}
};
