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
		window = glfwCreateWindow(800, 600, "Hyperworld", nullptr, nullptr);
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
			glfwSwapInterval(1);
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
		} else if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
			fullscreen = !fullscreen;

			if (fullscreen) {
				glfwGetWindowPos(window, &windowedXPos, &windowedYPos);
				glfwGetWindowSize(window, &windowedWidth, &windowedHeight);
				GLFWmonitor* monitor = glfwGetPrimaryMonitor();
				const GLFWvidmode* mode = glfwGetVideoMode(monitor);
				glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, 60);
			} else {
				glfwSetWindowMonitor(window, NULL, windowedXPos, windowedYPos, windowedWidth, windowedHeight, GLFW_DONT_CARE);
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
	bool fullscreen = false;
	int windowedXPos, windowedYPos, windowedWidth, windowedHeight;
};
