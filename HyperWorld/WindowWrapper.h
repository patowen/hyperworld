#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdexcept>
#include <Eigen/Dense>

#include "ShaderInterface.h"
#include "ModelBank.h"
#include "RenderContext.h"

class ContextWrapper;

using Eigen::Matrix4f;

class WindowWrapper
{
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
		float ratio = width / (float)height;

		glClear(GL_COLOR_BUFFER_BIT);

		context.setModelView(VectorMath::rotation({0, 0, 1}, glfwGetTime()));
		Matrix4d p;
		p << 1.f/ratio, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1;
		context.setProjection(p);

		context.useShader();
		context.setUniforms();
		context.render(ModelHandle::TRIANGLE);
	}

	void renderLoop() {
		ShaderInterface shaderInterface;
		ModelBank modelBank(shaderInterface);
		RenderContext context(shaderInterface, modelBank);

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