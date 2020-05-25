#pragma once
#include "VectorMath.h"
#include "UserInput.h"

class GhostCamera {
public:
	GhostCamera(): pos(Matrix4d::Identity()), vel(0, 0, 0), rotationLock(false), slow(false) {
	}

	void prepareCallbacks(InputListener& inputListener) {
		inputs.rotationLock.setCallback(inputListener, [this](){onToggleRotationLock();});
		inputs.toggleSpeed.setCallback(inputListener, [this](){onToggleSpeed();});
	}

	void onToggleRotationLock() {
		rotationLock = !rotationLock;
	}

	void onToggleSpeed() {
		slow = !slow;
	}

	void step(double dt, const UserInput& userInput) {
		Vector3d goalVel(0, 0, 0);
		if (userInput.isPressed(inputs.forwards)) {
			goalVel(2) -= 1;
		}
		if (userInput.isPressed(inputs.backwards)) {
			goalVel(2) += 1;
		}
		if (userInput.isPressed(inputs.left)) {
			goalVel(0) -= 1;
		}
		if (userInput.isPressed(inputs.right)) {
			goalVel(0) += 1;
		}
		if (userInput.isPressed(inputs.down)) {
			goalVel(1) -= 1;
		}
		if (userInput.isPressed(inputs.up)) {
			goalVel(1) += 1;
		}

		double norm = goalVel.norm();
		if (norm > 1) {
			goalVel /= norm;
		}
		goalVel *= slow ? 0.2 : 2;

		double maxChange = (slow ? 0.8 : 4) * dt;
		Vector3d velDiff = goalVel - vel;
		double velDiffNorm = velDiff.norm();
		if (velDiffNorm < maxChange) {
			vel = goalVel;
		} else {
			vel += velDiff / velDiffNorm * maxChange;
		}

		handleMovement(dt);

		Vector2d mouseLook = userInput.getMouseLook();
		pos *= VectorMath::rotation(Vector3d(1, 0, 0), -mouseLook(1) * 0.002);
		pos *= VectorMath::rotation(Vector3d(0, 1, 0), -mouseLook(0) * 0.002);

		double zRotation = 0;
		if (userInput.isPressed(inputs.clockwise)) {
			zRotation -= 1;
		}
		if (userInput.isPressed(inputs.counterclockwise)) {
			zRotation += 1;
		}
		pos *= VectorMath::rotation(Vector3d(0, 0, 1), zRotation * dt);

		pos = VectorMath::orthogonalizeGramSchmidt(pos);

		if (userInput.isPressed(inputs.goHome)) {
			pos = VectorMath::orthogonalizeWithSqrt(pos + Matrix4d::Identity() * dt);
		}
	}

	Matrix4d getTransform() {
		return VectorMath::isometricInverse(pos);
	}

private:
	Matrix4d pos;
	Vector3d vel; //Relative to camera
	bool rotationLock;
	bool slow;

	class Inputs {
	public:
		InputHandle forwards = MouseButton(GLFW_MOUSE_BUTTON_1);
		InputHandle backwards = MouseButton(GLFW_MOUSE_BUTTON_2);
		InputHandle left = KeyboardButton(GLFW_KEY_A);
		InputHandle right = KeyboardButton(GLFW_KEY_D);
		InputHandle up = KeyboardButton(GLFW_KEY_W);
		InputHandle down = KeyboardButton(GLFW_KEY_S);
		InputHandle clockwise = KeyboardButton(GLFW_KEY_E);
		InputHandle counterclockwise = KeyboardButton(GLFW_KEY_Q);

		InputHandle toggleSpeed = KeyboardButton(GLFW_KEY_LEFT_SHIFT);

		InputHandle rotationLock = KeyboardButton(GLFW_KEY_LEFT_CONTROL);
		InputHandle goHome = KeyboardButton(GLFW_KEY_HOME);
	};

	Inputs inputs;

	void handleMovement(double dt) {
		if (rotationLock) {
			pos *= VectorMath::displacement(Vector3d(0, 0, vel(2)) * dt);
			pos *= VectorMath::horoRotation(vel(0) * dt, vel(1) * dt);
		} else {
			pos *= VectorMath::displacement(vel * dt);
		}
	}
};
