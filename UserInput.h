#pragma once

#include <GLFW/glfw3.h>
#include <memory>

class MouseButton;
class KeyboardButton;

class InputListener {
public:
	void mouseButtonPressed(int button) {
		for (const auto& element : mouseCallbacks[button]) {
			element.second();
		}
	}

	void keyboardKeyPressed(int key) {
		for (const auto& element : keyboardCallbacks[key]) {
			element.second();
		}
	}

	~InputListener();

private:
	void addMouseCallback(int button, MouseButton* inputButton, const std::function<void()>& callback) {
		if (mouseCallbacks.find(button) == mouseCallbacks.end()) {
			mouseCallbacks[button] = std::unordered_map<MouseButton*, std::function<void()>>();
		}
		mouseCallbacks[button][inputButton] = callback;
	}

	void removeMouseCallback(int button, MouseButton* inputButton) {
		mouseCallbacks[button].erase(inputButton);
		if (mouseCallbacks[button].empty()) {
			mouseCallbacks.erase(button);
		}
	}

	void addKeyboardCallback(int key, KeyboardButton* inputButton, const std::function<void()>& callback) {
		if (keyboardCallbacks.find(key) == keyboardCallbacks.end()) {
			keyboardCallbacks[key] = std::unordered_map<KeyboardButton*, std::function<void()>>();
		}
		keyboardCallbacks[key][inputButton] = callback;
	}

	void removeKeyboardCallback(int key, KeyboardButton* inputButton) {
		keyboardCallbacks[key].erase(inputButton);
		if (keyboardCallbacks[key].empty()) {
			keyboardCallbacks.erase(key);
		}
	}

	std::unordered_map<int, std::unordered_map<MouseButton*, std::function<void()>>> mouseCallbacks;
	std::unordered_map<int, std::unordered_map<KeyboardButton*, std::function<void()>>> keyboardCallbacks;

	friend class MouseButton;
	friend class KeyboardButton;
};

class InputButton {
public:
	virtual ~InputButton() = default;

private:
	virtual bool isPressed(GLFWwindow* window) = 0;
	virtual void setCallback(InputListener& listener, const std::function<void()>& callback) = 0;

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

	void setCallback(InputListener& listener, const std::function<void()>& callback) {
	}
};

class MouseButton: public InputButton {
public:
	MouseButton(int button): button(button), listener(nullptr) {}
	~MouseButton() {
		if (listener != nullptr) {
			listener->removeMouseCallback(button, this);
		}
	}

	MouseButton(const MouseButton&) = delete;
	MouseButton& operator=(const MouseButton&) = delete;

	MouseButton(MouseButton&& other) noexcept :
			button(other.button),
			listener(other.listener) {
		other.listener = nullptr;
	}

	MouseButton&& operator=(MouseButton&& other) noexcept {
		button = other.button;
		listener = other.listener;
		other.listener = nullptr;
	}

private:
	bool isPressed(GLFWwindow* window) override {
		return glfwGetMouseButton(window, button) == GLFW_PRESS;
	}

	void setCallback(InputListener& listener, const std::function<void()>& callback) {
		this->listener = &listener;
		listener.addMouseCallback(button, this, callback);
	}

	int button;
	InputListener* listener;

	friend class InputListener;
};

class KeyboardButton : public InputButton {
public:
	KeyboardButton(int key): key(key), listener(nullptr) {}
	~KeyboardButton() {
		if (listener != nullptr) {
			listener->removeKeyboardCallback(key, this);
		}
	}

	KeyboardButton(const KeyboardButton&) = delete;
	KeyboardButton& operator=(const KeyboardButton&) = delete;

	KeyboardButton(KeyboardButton&& other) noexcept :
			key(other.key),
			listener(other.listener) {
		other.listener = nullptr;
	}

	KeyboardButton&& operator=(KeyboardButton&& other) noexcept {
		key = other.key;
		listener = other.listener;
		other.listener = nullptr;
	}

private:
	bool isPressed(GLFWwindow* window) override {
		return glfwGetKey(window, key) == GLFW_PRESS;
	}

	void setCallback(InputListener& listener, const std::function<void()>& callback) {
		this->listener = &listener;
		listener.addKeyboardCallback(key, this, callback);
	}

	int key;
	InputListener* listener;

	friend class InputListener;
};

InputListener::~InputListener() {
	for (auto& mc : mouseCallbacks) {
		for (auto& mc2 : mc.second) {
			mc2.first->listener = nullptr;
		}
	}

	for (auto& kc : keyboardCallbacks) {
		for (auto& kc2 : kc.second) {
			kc2.first->listener = nullptr;
		}
	}
}

class InputHandle {
public:
	InputHandle(): inputButton(std::make_unique<NullButton>()) {}

	template<typename T>
	InputHandle(T&& inputButton): inputButton(std::make_unique<T>(std::move(inputButton))) {}

	void setCallback(InputListener& listener, const std::function<void()>& callback) {
		inputButton->setCallback(listener, callback);
	}

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
