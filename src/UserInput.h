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

#include <GLFW/glfw3.h>
#include <memory>
#include <unordered_set>

class MouseButton;
class KeyboardButton;

class InputListener {
public:
	void mouseButtonPressed(int button) {
		mouseButtonsPressedThisStep.insert(button);
	}

	void keyboardKeyPressed(int key) {
		keyboardKeysPressedThisStep.insert(key);
	}

	void clearPressedThisStepList() {
		mouseButtonsPressedThisStep.clear();
		keyboardKeysPressedThisStep.clear();
	}

private:
	std::unordered_set<int> mouseButtonsPressedThisStep;
	std::unordered_set<int> keyboardKeysPressedThisStep;

	friend class MouseButton;
	friend class KeyboardButton;
};

class InputButton {
public:
	virtual ~InputButton() = default;

private:
	virtual bool isPressed(GLFWwindow* window) = 0;
	virtual bool pressedThisStep(const InputListener& listener) = 0;

	friend class NullButton;
	friend class MouseButton;
	friend class KeyboardButton;
	friend class UserInput;
	friend class InputHandle;
};

class NullButton: public InputButton {
public:
	NullButton() {}

private:
	bool isPressed(GLFWwindow* window) override {
		return false;
	}

	bool pressedThisStep(const InputListener& listener) override {
		return false;
	}

	void setCallback(InputListener& listener, const std::function<void()>& callback) {
	}
};

class MouseButton: public InputButton {
public:
	MouseButton(int button): button(button) {}

private:
	bool isPressed(GLFWwindow* window) override {
		return glfwGetMouseButton(window, button) == GLFW_PRESS;
	}

	bool pressedThisStep(const InputListener& listener) override {
		return listener.mouseButtonsPressedThisStep.find(button) != listener.mouseButtonsPressedThisStep.end();
	}

	int button;
};

class KeyboardButton : public InputButton {
public:
	KeyboardButton(int key): key(key) {}

private:
	bool isPressed(GLFWwindow* window) override {
		return glfwGetKey(window, key) == GLFW_PRESS;
	}

	bool pressedThisStep(const InputListener& listener) override {
		return listener.keyboardKeysPressedThisStep.find(key) != listener.keyboardKeysPressedThisStep.end();
	}

	int key;
};

class InputHandle {
public:
	InputHandle(): inputButton(std::make_unique<NullButton>()) {}

	template<typename T>
	InputHandle(T&& inputButton): inputButton(std::make_unique<T>(std::move(inputButton))) {}

private:
	std::unique_ptr<InputButton> inputButton;
	friend class UserInput;
};

class UserInput {
public:
	UserInput(GLFWwindow* window, const InputListener& inputListener, Vector2d mouseLook): window(window), inputListener(inputListener), mouseLook(mouseLook) {}

	bool isPressed(const InputHandle& inputHandle) const {
		return inputHandle.inputButton->isPressed(window);
	}

	bool pressedThisStep(const InputHandle& inputHandle) const {
		return inputHandle.inputButton->pressedThisStep(inputListener);
	}

	Vector2d getMouseLook() const {
		return mouseLook;
	}

private:
	GLFWwindow* window;
	const InputListener& inputListener;
	Vector2d mouseLook;
};
