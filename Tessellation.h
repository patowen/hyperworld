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
					if (faces[j].adjacentFaces[k].isNull()) {
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

	template<typename T>
	class PolarityArray {
	public:
		PolarityArray() {}
		PolarityArray(T elemPos, T elemNeg): data {elemPos, elemNeg} {}
		T& operator[](int pole) { return data[(1 - pole) / 2]; }
		const T& operator[](int pole) const { return data[(1 - pole) / 2]; }
	private:
		std::array<T, 2> data;
	};

	std::array<int, 2> poles = {1, -1};

	template<typename T>
	class VectorRef {
	public:
		VectorRef(): index(nullRef()) {}
		VectorRef(size_t index): index(index) {}
		bool isNull() { return index == nullRef(); }
		size_t index;
	private:
		static constexpr size_t nullRef() { return std::numeric_limits<size_t>::max(); }
	};

	class Face;
	class Vertex;
	std::vector<Face> faces;
	std::vector<Vertex> vertices;

	using FaceRef = VectorRef<Face>;
	using VertexRef = VectorRef<Vertex>;

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
			fanBoundaries(numAdjacentFaces - 1, 0) {
			adjacentFaces[numAdjacentFaces - 1] = startFace;
		}

		void addFace(FaceRef face, int direction) {
			if (isSaturated()) {
				throw std::runtime_error("Tried to add to a saturated vertex");
			}

			adjacentFaces[fanBoundaries[direction] + (-direction - 1) / 2] = face;
			fanBoundaries[direction] -= direction;
		}

		bool isSaturated() {
			return fanBoundaries[1] == fanBoundaries[-1];
		}

		FaceRef getFace(int direction) {
			return adjacentFaces[(fanBoundaries[direction] + adjacentFaces.size() + (direction - 1) / 2) % adjacentFaces.size()];
		}

		int type;
		std::vector<FaceRef> adjacentFaces;
		PolarityArray<unsigned> fanBoundaries;
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
				std::cout << adj.index << "(" << (getVertex(adj).fanBoundaries[1] - getVertex(adj).fanBoundaries[-1]) << "), ";
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

	void incrementIndex(unsigned& index, int pole) {
		index = (index + n + pole) % n;
	}

	FaceRef createAdjacentFace(FaceRef face, unsigned edge) {
		PolarityArray<unsigned> seedVertexIndices((edge + 2u) % n, (edge + 1u) % n);

		int orientation = -getFace(face).orientation;
		faces.emplace_back(orientation);
		FaceRef newFace(faces.size() - 1);

		// Sort out all direct adjacencies
		// Face to face
		getFace(face).adjacentFaces[edge] = newFace;
		getFace(newFace).adjacentFaces[edge] = face;

		// Face to vertex / vertex to face
		PolarityArray<VertexRef> seedVertices;
		for (int pole : poles) {
			seedVertices[pole] = getFace(face).adjacentVertices[seedVertexIndices[pole]];
			getFace(newFace).adjacentVertices[seedVertexIndices[pole]] = seedVertices[pole];
			getVertex(seedVertices[pole]).addFace(newFace, -pole * orientation);
		}

		// Vertex saturation prep
		PolarityArray<unsigned> walkingVertexIndices = seedVertexIndices;
		PolarityArray<VertexRef> walkingVertices = seedVertices;
		PolarityArray<unsigned> walkingEdges(edge, edge);

		// Vertex saturation
		for (int pole : poles) {
			while (walkingVertexIndices[pole] != walkingVertexIndices[-pole]) {
				if (!getVertex(walkingVertices[pole]).isSaturated()) {
					break;
				}
				FaceRef existingFace = getVertex(walkingVertices[pole]).getFace(pole * orientation);
				incrementIndex(walkingEdges[pole], pole);
				incrementIndex(walkingVertexIndices[pole], pole);
				walkingVertices[pole] = getFace(existingFace).adjacentVertices[walkingVertexIndices[pole]];

				getFace(newFace).adjacentFaces[walkingEdges[pole]] = existingFace;
				getFace(existingFace).adjacentFaces[walkingEdges[pole]] = newFace;

				if (getFace(newFace).adjacentVertices[walkingVertexIndices[pole]].isNull()) {
					getFace(newFace).adjacentVertices[walkingVertexIndices[pole]] = walkingVertices[pole];
					getVertex(walkingVertices[pole]).addFace(newFace, -pole * orientation);
				}
			}
		}

		// Freshly-created vertices
		if (walkingVertexIndices[1] != walkingVertexIndices[-1]) {
			for (unsigned newVertexIndex = (walkingVertexIndices[1] + 1u) % n; getFace(newFace).adjacentVertices[newVertexIndex].isNull(); incrementIndex(newVertexIndex, 1)) {
				vertices.emplace_back(newVertexIndex, shape[newVertexIndex] * 2, newFace);
				VertexRef newVertex(vertices.size() - 1);
				getFace(newFace).adjacentVertices[newVertexIndex] = newVertex;
			}
		}

		return newFace;
	}
};
