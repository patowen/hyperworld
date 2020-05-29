#pragma once

#include "glad.h"
#include <GLFW/glfw3.h>

#include <stdexcept>
#include "VectorMath.h"
#include "ShaderInterface.h"
#include "ModelBank.h"
#include "TextureBank.h"
#include "RenderContext.h"
#include "GhostCamera.h"
#include "UserInput.h"

class ContextWrapper;

class WindowWrapper {
public:
	WindowWrapper(const ContextWrapper &contextWrapper) : contextWrapper(contextWrapper), zoom(1) {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

		window = glfwCreateWindow(640, 480, "Hyperworld", nullptr, nullptr);
		if (!window) {
			throw std::runtime_error("Failed to create window");
		}
		glfwSetWindowUserPointer(window, this);

		glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			static_cast<WindowWrapper*>(glfwGetWindowUserPointer(window))->keyCallback(window, key, scancode, action, mods);
		});

		glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods) {
			static_cast<WindowWrapper*>(glfwGetWindowUserPointer(window))->mouseCallback(window, button, action, mods);
		});

		if (glfwRawMouseMotionSupported()) {
			glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
		}

		glfwMakeContextCurrent(window);
		gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);
		glfwSwapInterval(1);
	}

	~WindowWrapper() {
		glfwDestroyWindow(window);
	}

	void render(int width, int height, RenderContext &context) {
		double ratio = width / (double)height;

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Vector2d mouseLook(0, 0);
		if (isMouseCaptured()) {
			glfwGetCursorPos(window, &mouseLook(0), &mouseLook(1));
			glfwSetCursorPos(window, 0, 0);
		}

		UserInput userInput(window, mouseLook);
		camera.step(1.0/60.0, userInput);

		if (userInput.isPressed(KeyboardButton(GLFW_KEY_O))) {
			zoom *= 0.964;
		}

		if (userInput.isPressed(KeyboardButton(GLFW_KEY_P))) {
			zoom /= 0.964;
		}

		context.resetModelView();
		context.addModelView(camera.getTransform());
		context.addModelView(VectorMath::displacement({0.0, 0.0, -2.0}));
		context.setProjection(VectorMath::perspective(ratio * zoom, zoom, 0.01, 10));

		context.useShader();
		context.setUniforms();
		context.setTexture(TextureHandle::TILE);
		context.render(ModelHandle::HOROSPHERE);
		context.setTexture(TextureHandle::PERLIN);
		context.addModelView(VectorMath::rotation(Vector3d(1, 0, 0), M_TAU / 4.0));
		context.setUniforms();
		//context.render(ModelHandle::DODECAHEDRON);
		context.render(ModelHandle::PLANE);
	}

	void renderLoop() {
		ShaderInterface shaderInterface;
		ModelBank modelBank(shaderInterface);
		TextureBank textureBank;
		RenderContext context(shaderInterface, modelBank, textureBank);

		camera.prepareCallbacks(inputListener);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_FRAMEBUFFER_SRGB);

		while (!glfwWindowShouldClose(window)) {
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);
			glViewport(0, 0, width, height);
			render(width, height, context);
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
		if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
			if (isMouseCaptured()) {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			} else {
				glfwSetWindowShouldClose(window, GLFW_TRUE);
			}
		} else if (action == GLFW_PRESS) {
			inputListener.keyboardKeyPressed(key);
		}
	}

	void mouseCallback(GLFWwindow* window, int button, int action, int mods) {
		if (!isMouseCaptured()) {
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			glfwSetCursorPos(window, 0, 0);
		} else if (action == GLFW_PRESS) {
			inputListener.mouseButtonPressed(button);
		}
	}

	bool isMouseCaptured() {
		return glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED;
	}

	WindowWrapper(const WindowWrapper&) = delete;
	WindowWrapper& operator=(const WindowWrapper&) = delete;

private:
	GLFWwindow* window;
	const ContextWrapper &contextWrapper;
	GhostCamera camera;
	InputListener inputListener;
	double zoom;
};
