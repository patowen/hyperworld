#pragma once

#include "glad.h"
#include <GLFW/glfw3.h>
#include <stdexcept>
#include <string>

class ContextWrapper {
public:
	ContextWrapper() {
		glfwSetErrorCallback([](int error, const char* description) {
			fprintf(stderr, "Error: %s\n", description);
			std::string full_description = "Error " + std::to_string(error) + ": " + std::string(description);
			throw std::runtime_error(full_description.c_str());
		});

		int success = glfwInit();
		if (!success) {
			throw std::runtime_error("Failed to initialize GLFW");
		}
	}

	~ContextWrapper() {
		glfwTerminate();
	}

	ContextWrapper(const ContextWrapper&) = delete;
	ContextWrapper& operator=(const ContextWrapper&) = delete;
};