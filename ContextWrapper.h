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