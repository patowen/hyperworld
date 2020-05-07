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
					if (faces[j]->adjacentFaces[k] == nullptr) {
						Face& face = createAdjacentFace(*faces[j], k);
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

	class Face;
	class Vertex;
	std::vector<std::unique_ptr<Face>> faces;
	std::vector<std::unique_ptr<Vertex>> vertices;

	class Face {
	public:
		Face(int orientation): orientation(orientation) {
			adjacentFaces.fill(nullptr);
			adjacentVertices.fill(nullptr);
		}
		int orientation;
		std::array<Face*, n> adjacentFaces;
		std::array<Vertex*, n> adjacentVertices;
	};

	class Vertex {
	public:
		Vertex(unsigned type, unsigned numAdjacentFaces, Face& startFace):
			type(type),
			adjacentFaces(numAdjacentFaces, nullptr),
			fanBoundaries(numAdjacentFaces - 1, 0) {
			adjacentFaces[numAdjacentFaces - 1] = &startFace;
		}

		void addFace(Face& face, int direction) {
			if (isSaturated()) {
				throw std::runtime_error("Tried to add to a saturated vertex");
			}

			adjacentFaces[fanBoundaries[direction] + (-direction - 1) / 2] = &face;
			fanBoundaries[direction] -= direction;
		}

		bool isSaturated() {
			return fanBoundaries[1] == fanBoundaries[-1];
		}

		Face& getFace(int direction) {
			return *adjacentFaces[(fanBoundaries[direction] + adjacentFaces.size() + (direction - 1) / 2) % adjacentFaces.size()];
		}

		int type;
		std::vector<Face*> adjacentFaces;
		PolarityArray<unsigned> fanBoundaries;
	};

	Face& createSeedFace() {
		std::unique_ptr<Face> newFacePtr = std::make_unique<Face>(1);
		Face& newFace = *newFacePtr;

		for (unsigned newVertexIndex = 0; newVertexIndex != n; ++newVertexIndex) {
			std::unique_ptr<Vertex> newVertex = std::make_unique<Vertex>(newVertexIndex, shape[newVertexIndex] * 2, newFace);
			newFace.adjacentVertices[newVertexIndex] = newVertex.get();
			vertices.push_back(std::move(newVertex));
		}

		faces.push_back(std::move(newFacePtr));
		return newFace;
	}

	void incrementIndex(unsigned& index, int pole) {
		index = (index + n + pole) % n;
	}

	Face& createAdjacentFace(Face& face, unsigned edge) {
		PolarityArray<unsigned> seedVertexIndices((edge + 2u) % n, (edge + 1u) % n);

		int orientation = -face.orientation;
		std::unique_ptr<Face> newFacePtr = std::make_unique<Face>(orientation);
		Face& newFace = *newFacePtr;

		// Sort out all direct adjacencies
		// Face to face
		face.adjacentFaces[edge] = &newFace;
		newFace.adjacentFaces[edge] = &face;

		// Face to vertex / vertex to face
		PolarityArray<Vertex*> seedVertices;
		for (int pole : poles) {
			seedVertices[pole] = face.adjacentVertices[seedVertexIndices[pole]];
			newFace.adjacentVertices[seedVertexIndices[pole]] = seedVertices[pole];
			seedVertices[pole]->addFace(newFace, -pole * orientation);
		}

		// Vertex saturation
		// TODO: This logic is a little shaky, and the code is nigh unreadable. The purpose of the vertex
		// saturation section is to walk around the edges of the face checking to see if the neighboring face
		// has already been discovered. This should be true iff the an associated vertex is saturated, meaning
		// that all faces around that vertex have been discovered.
		PolarityArray<unsigned> walkingVertexIndices = seedVertexIndices;
		PolarityArray<Vertex*> walkingVertices = seedVertices;
		PolarityArray<unsigned> walkingEdges(edge, edge);

		for (int pole : poles) {
			while (walkingVertexIndices[pole] != walkingVertexIndices[-pole]) {
				if (!walkingVertices[pole]->isSaturated()) {
					break;
				}
				Face& existingFace = walkingVertices[pole]->getFace(pole * orientation);
				incrementIndex(walkingEdges[pole], pole);
				incrementIndex(walkingVertexIndices[pole], pole);
				walkingVertices[pole] = existingFace.adjacentVertices[walkingVertexIndices[pole]];

				newFace.adjacentFaces[walkingEdges[pole]] = &existingFace;
				existingFace.adjacentFaces[walkingEdges[pole]] = &newFace;

				if (newFace.adjacentVertices[walkingVertexIndices[pole]] == nullptr) {
					newFace.adjacentVertices[walkingVertexIndices[pole]] = walkingVertices[pole];
					walkingVertices[pole]->addFace(newFace, -pole * orientation);
				}
			}
		}

		// Freshly-created vertices
		for (unsigned newVertexIndex = (walkingVertexIndices[1] + 1u) % n; newFace.adjacentVertices[newVertexIndex] == nullptr; incrementIndex(newVertexIndex, 1)) {
			std::unique_ptr<Vertex> newVertex = std::make_unique<Vertex>(newVertexIndex, shape[newVertexIndex] * 2, newFace);
			newFace.adjacentVertices[newVertexIndex] = newVertex.get();
			vertices.push_back(std::move(newVertex));
		}

		faces.push_back(std::move(newFacePtr));
		return newFace;
	}
};
