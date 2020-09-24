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
#include "VectorMath.h"
#include "UserInput.h"
#include "Entity.h"

class GhostCamera : public Entity {
public:
	GhostCamera(): pos(Matrix4d::Identity()), vel(0, 0, 0, 0), zoom(1), rotationLock(false), slow(false) {
	}

	void step(double dt, const UserInput& userInput) override {
		setSwitchesFromInput(dt, userInput);
		setRotationFromInput(dt, userInput);
		setVelocityFromInput(dt, userInput);
		setPositionFromVelocity(dt);
		setPositionFromInput(dt, userInput);
		renormalize();
	}

	Matrix4d getCameraPos() {
		return pos;
	}

	double getCameraZoom() {
		return zoom;
	}

private:
	Matrix4d pos;
	Vector4d vel; //Relative to camera
	double zoom;
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

	void setSwitchesFromInput(double dt, const UserInput& userInput) {
		if (userInput.pressedThisStep(inputs.rotationLock)) {
			rotationLock = !rotationLock;
		}

		if (userInput.pressedThisStep(inputs.toggleSpeed)) {
			slow = !slow;
		}

		if (userInput.isPressed(KeyboardButton(GLFW_KEY_O))) {
			zoom *= 0.964;
		}

		if (userInput.isPressed(KeyboardButton(GLFW_KEY_P))) {
			zoom /= 0.964;
		}
	}

	void setRotationFromInput(double dt, const UserInput& userInput) {
		Vector2d mouseLook = userInput.getMouseLook();
		Matrix4d rotation = Matrix4d::Identity();
		rotation *= VectorMath::rotation(Vector3d(1, 0, 0), -mouseLook(1) * 0.002 * zoom);
		rotation *= VectorMath::rotation(Vector3d(0, 1, 0), -mouseLook(0) * 0.002 * zoom);

		double zRotation = 0;
		if (userInput.isPressed(inputs.clockwise)) {
			zRotation -= 1;
		}
		if (userInput.isPressed(inputs.counterclockwise)) {
			zRotation += 1;
		}
		rotation *= VectorMath::rotation(Vector3d(0, 0, 1), zRotation * dt);

		pos *= rotation;
		vel = VectorMath::isometricInverse(rotation) * vel;
	}

	void setVelocityFromInput(double dt, const UserInput& userInput) {
		Vector4d goalVel(0, 0, 0, 0);
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
		Vector4d velDiff = goalVel - vel;
		double velDiffNorm = velDiff.norm();
		if (velDiffNorm < maxChange) {
			vel = goalVel;
		} else {
			vel += velDiff / velDiffNorm * maxChange;
		}
	}

	void setPositionFromVelocity(double dt) {
		if (rotationLock) {
			pos *= VectorMath::displacement(Vector4d(0, 0, vel(2), 0) * dt);
			pos *= VectorMath::horoRotation(vel(0) * dt, vel(1) * dt);
		} else {
			pos *= VectorMath::displacement(vel * dt);
		}
	}

	void setPositionFromInput(double dt, const UserInput& userInput) {
		if (userInput.isPressed(inputs.goHome)) {
			pos = VectorMath::orthogonalizeWithSqrt(pos + Matrix4d::Identity() * dt);
		}
	}

	void renormalize() {
		pos = VectorMath::orthogonalizeGramSchmidt(pos);
	}
};
