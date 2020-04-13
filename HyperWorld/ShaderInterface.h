#pragma once
#include <glad/glad.h>
#include "VectorMath.h"

class ShaderInterface {
public:
	ShaderInterface() {
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexShaderText, NULL);
		glCompileShader(vertexShader);

		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentShaderText, NULL);
		glCompileShader(fragmentShader);

		program = glCreateProgram();
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);
		glLinkProgram(program);

		mvpLocation = glGetUniformLocation(program, "MVP");
		vPosLocation = glGetAttribLocation(program, "vPos");
		vNormalLocation = glGetAttribLocation(program, "vNormal");
		vTexCoordLocation = glGetAttribLocation(program, "vTexCoord");
	}

	~ShaderInterface() {
		glDeleteProgram(program);
		glDeleteShader(fragmentShader);
		glDeleteShader(vertexShader);
	}

	void use() {
		glUseProgram(program);
	}

	void setMvp(const Eigen::Matrix4f& mvp) {
		glUniformMatrix4fv(mvpLocation, 1, GL_FALSE, (const GLfloat*) mvp.data());
	}

	ShaderInterface(const ShaderInterface&) = delete;
	ShaderInterface& operator=(const ShaderInterface&) = delete;

private:
	static const char* vertexShaderText;
	static const char* fragmentShaderText;

	GLuint vertexShader, fragmentShader, program;
	GLint mvpLocation, vPosLocation, vNormalLocation, vTexCoordLocation;
	friend class Model;
};

const char* ShaderInterface::vertexShaderText = "#version 150\n"
	"uniform mat4 MVP;\n"
	"in vec4 vPos;\n"
	"in vec4 vNormal;\n"
	"in vec2 vTexCoord;\n"
	"out vec4 normal;\n"
	"out vec2 texCoord;\n"
	"void main()\n"
	"{\n"
	"    gl_Position = MVP * vPos;\n"
	"    normal = vNormal;\n"
	"    texCoord = vTexCoord;\n"
	"}\n";

const char* ShaderInterface::fragmentShaderText =
	"#version 150\n"
	"in vec4 normal;\n"
	"in vec2 texCoord;\n"
	"out vec4 fragColor;"
	"void main()\n"
	"{\n"
	"    fragColor = vec4(mod(floor(texCoord * 10), 2) * 0.5 + 0.5, 0.0, 1.0);\n"
	"}\n";
