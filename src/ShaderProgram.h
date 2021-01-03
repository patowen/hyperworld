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
#include "glad.h"
#include "VectorMath.h"
#include "Model.h"
#include "ShaderProgramData.h"

class ShaderProgram {
public:
	ShaderProgram(const ShaderProgramData& data) {
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
		const char *vertexShaderText = data.vertexShaderText.c_str();
		glShaderSource(vertexShader, 1, &vertexShaderText, NULL);
		glCompileShader(vertexShader);

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		const char* fragmentShaderText = data.fragmentShaderText.c_str();
		glShaderSource(fragmentShader, 1, &fragmentShaderText, NULL);
		glCompileShader(fragmentShader);

		shaderProgramRef = glCreateProgram();
		glAttachShader(shaderProgramRef, vertexShader);
		glAttachShader(shaderProgramRef, fragmentShader);

		glBindAttribLocation(shaderProgramRef, Model::vPosLocation, "vPos");
		glBindAttribLocation(shaderProgramRef, Model::vNormalLocation, "vNormal");
		glBindAttribLocation(shaderProgramRef, Model::vTexCoordLocation, "vTexCoord");

		glLinkProgram(shaderProgramRef);

		glDetachShader(shaderProgramRef, vertexShader);
		glDetachShader(shaderProgramRef, fragmentShader);
		glDeleteShader(vertexShader);
		glDeleteShader(fragmentShader);

		GLint isLinked = 0;
		glGetProgramiv(shaderProgramRef, GL_LINK_STATUS, &isLinked);
		if (!isLinked) {
			glDeleteProgram(shaderProgramRef);
			throw std::runtime_error("Shader Compile/Link Error");
		}

		projectionLocation = glGetUniformLocation(shaderProgramRef, "projection");
		modelViewLocation = glGetUniformLocation(shaderProgramRef, "modelView");
		lightPosLocation = glGetUniformLocation(shaderProgramRef, "light");
	}

	~ShaderProgram() {
		glDeleteProgram(shaderProgramRef);
	}

	void use() {
		glUseProgram(shaderProgramRef);
	}

	void setProjection(const Eigen::Matrix4f& projection) {
		glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, (const GLfloat*) projection.data());
	}

	void setModelView(const Eigen::Matrix4f& modelView) {
		glUniformMatrix4fv(modelViewLocation, 1, GL_FALSE, (const GLfloat*) modelView.data());
	}

	void setLightPos(const Eigen::Vector4f& lightPos) {
		glUniform4fv(lightPosLocation, 1, (const GLfloat*) lightPos.data());
	}

	ShaderProgram(const ShaderProgram&) = delete;
	ShaderProgram& operator=(const ShaderProgram&) = delete;

private:
	GLuint shaderProgramRef;
	GLint projectionLocation, modelViewLocation, lightPosLocation;
};
