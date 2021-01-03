/*
	Copyright 2021 Patrick Owen

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
#include <fstream>
#include <sstream>
#include "ShaderProgramData.h"

class ShaderProgramLoader {
public:
	static ShaderProgramData loadShaderProgram(const std::string& name) {
		std::string vertexShaderPath = "shaders/" + name + ".vert";
		std::ifstream vertexShaderFstream(vertexShaderPath);
		std::stringstream vertexShaderStringStream;
		vertexShaderStringStream << vertexShaderFstream.rdbuf();

		std::string fragmentShaderPath = "shaders/" + name + ".frag";
		std::ifstream fragmentShaderFstream(fragmentShaderPath);
		std::stringstream fragmentShaderStringStream;
		fragmentShaderStringStream << fragmentShaderFstream.rdbuf();

		return ShaderProgramData(vertexShaderStringStream.str(), fragmentShaderStringStream.str());
	}
};
