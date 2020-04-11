#pragma once
#include <glad/glad.h>
#include <Eigen/Dense>

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
		vposLocation = glGetAttribLocation(program, "vPos");
		vcolLocation = glGetAttribLocation(program, "vCol");
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
	GLint mvpLocation, vposLocation, vcolLocation;
	friend class Model;
};

const char* ShaderInterface::vertexShaderText = "#version 150\n"
	"uniform mat4 MVP;\n"
	"in vec3 vCol;\n"
	"in vec2 vPos;\n"
	"out vec3 color;\n"
	"void main()\n"
	"{\n"
	"    gl_Position = MVP * vec4(vPos, -5.0, 1.0);\n"
	"    color = vCol;\n"
	"}\n";

const char* ShaderInterface::fragmentShaderText =
	"#version 150\n"
	"in vec3 color;\n"
	"out vec4 fragColor;"
	"void main()\n"
	"{\n"
	"    fragColor = vec4(color, 1.0);\n"
	"}\n";
