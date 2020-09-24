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
#include <array>
#include <limits>
#include <iostream>
#include "VectorMath.h"

class Tessellation {
public:
	static constexpr unsigned n = 3;

	Tessellation() {
		std::array<double, n> angles;
		std::array<double, n> cosAngles;
		std::array<double, n> sinAngles;
		for (int i=0; i<n; ++i) {
			angles[i] = M_TAU / shape[i] / 2;
			cosAngles[i] = cos(angles[i]);
			sinAngles[i] = sin(angles[i]);
		}

		faceVertices[0] = Vector4d(0, 0, 0, 1);

		double coshEdge01 = (cosAngles[2] + cosAngles[0] * cosAngles[1]) / (sinAngles[0] * sinAngles[1]);
		double sinhEdge01 = sqrt(coshEdge01 * coshEdge01 - 1);
		faceVertices[1] = Vector4d(sinhEdge01, 0, 0, coshEdge01);

		double coshEdge02 = (cosAngles[1] + cosAngles[0] * cosAngles[2]) / (sinAngles[0] * sinAngles[2]);
		double sinhEdge02 = sqrt(coshEdge02 * coshEdge02 - 1);
		faceVertices[2] = Vector4d(sinhEdge02 * cosAngles[0], sinhEdge02 * sinAngles[0], 0, coshEdge02);

		for (int i=0; i<n; ++i) {
			Vector4d normal = VectorMath::hyperbolicNormal(faceVertices[(i+1) % n], faceVertices[(i+2) % n], Vector4d::UnitZ());
			normal /= sqrt(VectorMath::hyperbolicSqrNorm(normal));
			reflections[i] = VectorMath::reflection(normal);
		}
	}

	size_t getNumFaces() {
		return faces.size();
	}

	Vector4d getVertexPos(size_t faceIndex, size_t vertexIndex) {
		return faces[faceIndex]->adjacentVertices[vertexIndex]->pos;
	}

	int getOrientation(size_t faceIndex) {
		return faces[faceIndex]->orientation;
	}

	void testTessellation() {
		Face* currentFace = &createSeedFace();
		unsigned nextEdge = 1;
		while (currentFace->adjacentFaces[nextEdge] == nullptr) {
			currentFace = &createAdjacentFace(*currentFace, nextEdge);
			nextEdge = 3u - nextEdge;
		}

		for (unsigned i=0; i<18; ++i) {
			size_t currentCount = faces.size();

			for (size_t j=0; j<currentCount; ++j) {
				for (unsigned k=0; k<n; ++k) {
					if (faces[j]->adjacentFaces[k] == nullptr) {
						Face& face = createAdjacentFace(*faces[j], k);
					}
				}
			}
		}
	}

private:
	std::array<unsigned, n> shape = {2, 4, 5}; // {2, 4, 5}
	std::array<Vector4d, n> faceVertices;
	std::array<Matrix4d, n> reflections;

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
		Face(int orientation, Matrix4d pos): orientation(orientation), pos(pos) {
			adjacentFaces.fill(nullptr);
			adjacentVertices.fill(nullptr);
		}
		int orientation;
		Matrix4d pos;
		std::array<Face*, n> adjacentFaces;
		std::array<Vertex*, n> adjacentVertices;
	};

	class Vertex {
	public:
		Vertex(unsigned type, unsigned numAdjacentFaces, Face& startFace, Vector4d pos):
			type(type),
			adjacentFaces(numAdjacentFaces, nullptr),
			fanBoundaries(numAdjacentFaces - 1, 0),
			pos(pos) {
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
		Vector4d pos;
	};

	Face& createSeedFace() {
		std::unique_ptr<Face> newFacePtr = std::make_unique<Face>(1, Matrix4d::Identity());
		Face& newFace = *newFacePtr;

		for (unsigned newVertexIndex = 0; newVertexIndex != n; ++newVertexIndex) {
			std::unique_ptr<Vertex> newVertex = std::make_unique<Vertex>(newVertexIndex, shape[newVertexIndex] * 2, newFace, faceVertices[newVertexIndex]);
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
		std::unique_ptr<Face> newFacePtr = std::make_unique<Face>(orientation, face.pos * reflections[edge]);
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
			std::unique_ptr<Vertex> newVertex = std::make_unique<Vertex>(newVertexIndex, shape[newVertexIndex] * 2, newFace, newFace.pos * faceVertices[newVertexIndex]);
			newFace.adjacentVertices[newVertexIndex] = newVertex.get();
			vertices.push_back(std::move(newVertex));
		}

		faces.push_back(std::move(newFacePtr));
		return newFace;
	}
};
