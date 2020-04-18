#pragma once
#include "VectorMath.h"
#include "UserInput.h"

class GhostCamera {
public:
	GhostCamera(): pos(Matrix4d::Identity()), vel(0, 0, 1) {
	}

	void step(double dt, const UserInput& userInput) {
		if (userInput.isPressed(inputs.backwards)) {
			pos *= VectorMath::displacement(vel * dt);
		}
	}

	Matrix4d getTransform() {
		return VectorMath::isometricInverse(pos);
	}

private:
	Matrix4d pos;
	Vector3d vel; //Relative to camera

	class Inputs {
	public:
		InputHandle forwards = MouseButton(GLFW_MOUSE_BUTTON_1);
		InputHandle backwards = MouseButton(GLFW_MOUSE_BUTTON_2);
		InputHandle left = KeyboardButton(GLFW_KEY_A);
		InputHandle right = KeyboardButton(GLFW_KEY_D);
		InputHandle up = KeyboardButton(GLFW_KEY_W);
		InputHandle down = KeyboardButton(GLFW_KEY_S);
	};

	Inputs inputs;
};
