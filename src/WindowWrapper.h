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
#include "ShaderProgramBank.h"
#include "ModelBank.h"
#include "TextureBank.h"
#include "RenderContext.h"
#include "GhostCamera.h"
#include "UserInput.h"
#include "Scene.h"
#include "SimpleRenderNode.h"
#include "SimpleSpawner.h"

#define GLT_IMPLEMENTATION
#include "gltext.h"

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
		ModelBank modelBank;
		TextureBank textureBank;
		ShaderProgramBank shaderProgramBank;
		RenderContext context(shaderProgramBank, modelBank, textureBank);

		camera.setSpherical(false);
		context.setSpherical(false);

		scene.setCamera(camera);
		scene.addEntity(camera);
		scene.addEntity(simpleSpawner);

		double previousFrameTime = 0;

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_FRAMEBUFFER_SRGB);
		glEnable(GL_CULL_FACE);

		// Initialize glText
		gltInit();

		// Creating text
		GLTtext *text = gltCreateText();


		while (!glfwWindowShouldClose(window)) {
			Vector2d mouseLook(0, 0);
			if (isMouseCaptured()) {
				glfwGetCursorPos(window, &mouseLook(0), &mouseLook(1));
				glfwSetCursorPos(window, 0, 0);
			}

			UserInput userInput(window, inputListener, mouseLook);

			double currentFrameTime = glfwGetTime();
			if (currentFrameTime != 0) {
				if (previousFrameTime != 0) {
					// Get amount of time passed since last frame
					double timeInFrame = currentFrameTime - previousFrameTime;
					if (timeInFrame > 0.05) {
						// Don't advance over 1/20 of a second in a single frame
						timeInFrame = 0.05;
					}

					// Advance the scene by the amount of time passed since last frame
					scene.step(timeInFrame, userInput);
					inputListener.clearPressedThisStepList();
				}
				previousFrameTime = currentFrameTime;
			}

			int width, height;
			glfwGetFramebufferSize(window, &width, &height);
			glViewport(0, 0, width, height);
			context.setDimensions(width, height);
			scene.render(context);

			Eigen::Matrix4d pos = camera.getPos();
			// Extracting the translation part (3D position vector)
			Eigen::Vector3d position = pos.block<3,1>(0, 3);

			// Convert the 3D vector to a string
			std::ostringstream oss;
			oss << position(0) << ", " << position(1) << ", " << position(2);
			std::string positionStr = oss.str();

			// Convert to char*
			char* positionChar = new char[positionStr.length() + 1];
			std::strcpy(positionChar, positionStr.c_str());
			gltSetText(text, positionChar);

			// Begin text drawing (this for instance calls glUseProgram)
			gltBeginDraw();

			// Draw any amount of text between begin and end
			gltColor(1.0f, 1.0f, 1.0f, 1.0f);

			gltDrawText2D(text, 16, 16, 2);
			delete[] positionChar;
			// Finish drawing text

			gltEndDraw();
			glfwSwapInterval(1);
			glfwSwapBuffers(window);

			glfwPollEvents();
		}


		// Deleting text
		gltDeleteText(text);

		// Destroy glText
		gltTerminate();
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
				glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, GLFW_DONT_CARE);
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
