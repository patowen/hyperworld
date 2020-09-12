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
#include "Scene.h"
#include "SimpleRenderNode.h"
#include "SimpleSpawner.h"

class ContextWrapper;

class WindowWrapper {
public:
	WindowWrapper(const ContextWrapper &contextWrapper) : contextWrapper(contextWrapper) {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

		glfwWindowHint(GLFW_SAMPLES, 4);
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

	void renderLoop() {
		Scene scene;
		GhostCamera camera;
		SimpleSpawner simpleSpawner(scene, camera);
		ShaderInterface shaderInterface;
		ModelBank modelBank(shaderInterface);
		TextureBank textureBank;
		RenderContext context(shaderInterface, modelBank, textureBank);

		scene.setCamera(camera);
		scene.addEntity(camera);
		scene.addEntity(simpleSpawner);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_FRAMEBUFFER_SRGB);
		glEnable(GL_CULL_FACE);

		while (!glfwWindowShouldClose(window)) {
			Vector2d mouseLook(0, 0);
			if (isMouseCaptured()) {
				glfwGetCursorPos(window, &mouseLook(0), &mouseLook(1));
				glfwSetCursorPos(window, 0, 0);
			}

			UserInput userInput(window, inputListener, mouseLook);
			scene.step(1.0/60.0, userInput);
			inputListener.clearPressedThisStepList();

			int width, height;
			glfwGetFramebufferSize(window, &width, &height);
			glViewport(0, 0, width, height);
			context.setDimensions(width, height);
			scene.render(context);
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
	InputListener inputListener;
};
