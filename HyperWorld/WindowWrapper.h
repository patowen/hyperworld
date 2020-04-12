#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdexcept>
#include "VectorMath.h"
#include "ShaderInterface.h"
#include "ModelBank.h"
#include "RenderContext.h"

class ContextWrapper;

class WindowWrapper {
public:
	WindowWrapper(const ContextWrapper &contextWrapper) : contextWrapper(contextWrapper) {
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

		window = glfwCreateWindow(640, 480, "Simple example", nullptr, nullptr);
		if (!window) {
			throw std::runtime_error("Failed to create window");
		}
		glfwSetWindowUserPointer(window, this);

		glfwSetKeyCallback(window, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
				glfwSetWindowShouldClose(window, GLFW_TRUE);
		});

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

		context.setModelView(VectorMath::euclideanTranslation({0.0, 0.0, -2.0}) * VectorMath::rotation({0, .6, .8}, glfwGetTime()));
		context.setProjection(VectorMath::perspective(ratio, 1, 1, 100));

		context.useShader();
		context.setUniforms();
		context.render(ModelHandle::TRIANGLE);
	}

	void renderLoop() {
		ShaderInterface shaderInterface;
		ModelBank modelBank(shaderInterface);
		RenderContext context(shaderInterface, modelBank);

		glEnable(GL_DEPTH_TEST);

		while (!glfwWindowShouldClose(window)) {
			int width, height;

			glfwGetFramebufferSize(window, &width, &height);
			glViewport(0, 0, width, height);
			render(width, height, context);
			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	WindowWrapper(const WindowWrapper&) = delete;
	WindowWrapper& operator=(const WindowWrapper&) = delete;

private:
	GLFWwindow* window;
	const ContextWrapper &contextWrapper;
};
