#pragma once
#include "VectorMath.h"

class GhostCamera {
public:
	GhostCamera(): pos(Matrix4d::Identity()), vel(Vector3d::Zero()) {}

	void step(double dt) {
		pos *= VectorMath::displacement(vel * dt);
	}

private:
	Matrix4d pos;
	Vector3d vel; //Relative to camera
};
