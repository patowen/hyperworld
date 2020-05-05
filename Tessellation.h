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
	template<typename T>
	class PolarityArray {
	public:
		PolarityArray(T elemPos, T elemNeg): data {elemPos, elemNeg} {}
		T& operator[](int pole) { return data[(1 - pole) / 2]; }
		const T& operator[](int pole) const { return data[(pole + 1) / 2]; }

		template<typename Ret, typename F>
		auto select(F f) -> PolarityArray<Ret> {
			return PolarityArray<Ret>(f(data[0]), f(data[1]));
		}
	private:
		std::array<T, 2> data;
	};

	std::array<int, 2> poles = {1, -1};

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

	void incrementIndex(unsigned& index, int pole) {
		index = (index + n + pole) % n;
	}

	FaceRef createAdjacentFace(FaceRef face, unsigned edge) {
		PolarityArray<unsigned> seedVertexIndices((edge + 2u) % n, (edge + 1u) % n);

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

		// Face to vertex / vertex to face
		auto seedVertices = seedVertexIndices.select<VertexRef>([this, face](auto index){ return getFace(face).adjacentVertices[index]; });
		for (int pole : poles) {
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
				getFace(newFace).adjacentVertices[walkingVertexIndices[pole]] = walkingVertices[pole];
				if (walkingVertexIndices[pole] != walkingVertexIndices[-pole]) {
					getVertex(walkingVertices[pole]).addFace(newFace, -pole * orientation);
				}
			}
		}

		// Freshly-created vertices
		if (walkingVertexIndices[1] != walkingVertexIndices[-1]) {
			for (unsigned newVertexIndex = (walkingVertexIndices[1] + 1u) % n; newVertexIndex != walkingVertexIndices[-1]; incrementIndex(newVertexIndex, 1)) {
				vertices.emplace_back(newVertexIndex, shape[newVertexIndex] * 2, newFace);
				VertexRef newVertex(vertices.size() - 1);
				getFace(newFace).adjacentVertices[newVertexIndex] = newVertex;
			}
		}

		return newFace;
	}
};
