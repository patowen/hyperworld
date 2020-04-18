#pragma once

#include <GLFW/glfw3.h>
#include <memory>

class InputButton {
private:
	virtual bool isPressed(GLFWwindow* window) = 0;

	friend class NullButton;
	friend class MouseButton;
	friend class KeyboardButton;
	friend class UserInput;
};

class NullButton: public InputButton {
public:
	NullButton() {}

private:
	bool isPressed(GLFWwindow* window) override {
		return false;
	}
};

class MouseButton: public InputButton {
public:
	MouseButton(int button): button(button) {}

private:
	bool isPressed(GLFWwindow* window) override {
		return glfwGetMouseButton(window, button) == GLFW_PRESS;
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

	int key;
};

class InputHandle {
public:
	InputHandle(): inputButton(std::make_unique<NullButton>()) {}

	template<typename T>
	InputHandle(const T& inputButton): inputButton(std::make_unique<T>(inputButton)) {}

private:
	std::unique_ptr<InputButton> inputButton;

	InputHandle(std::unique_ptr<InputButton>&& inputButton): inputButton(std::move(inputButton)) {}

	friend class UserInput;
};

class UserInput {
public:
	UserInput(GLFWwindow* window, Vector2d mouseLook): window(window), mouseLook(mouseLook) {}

	bool isPressed(const InputHandle& inputHandle) const {
		return inputHandle.inputButton->isPressed(window);
	}

	Vector2d getMouseLook() const {
		return mouseLook;
	}

private:
	GLFWwindow* window;
	Vector2d mouseLook;
};
