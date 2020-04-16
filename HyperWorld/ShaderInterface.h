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

		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &isLinked);
		if (isLinked == GL_FALSE) {
			glDeleteProgram(program);
			glDeleteShader(fragmentShader);
			glDeleteShader(vertexShader);

			throw std::runtime_error("Shader Link Error");
		}

		mvpLocation = glGetUniformLocation(program, "MVP");
		lightPosLocation = glGetUniformLocation(program, "light_pos");
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

	void setLightPos(const Eigen::Vector4f& lightPos) {
		glUniform4fv(lightPosLocation, 1, (const GLfloat*) lightPos.data());
	}

	ShaderInterface(const ShaderInterface&) = delete;
	ShaderInterface& operator=(const ShaderInterface&) = delete;

private:
	static const char* vertexShaderText;
	static const char* fragmentShaderText;

	GLuint vertexShader, fragmentShader, program;
	GLint mvpLocation, lightPosLocation, vPosLocation, vNormalLocation, vTexCoordLocation;
	friend class Model;
};

const char* ShaderInterface::vertexShaderText = "#version 150\n"
	"uniform mat4 MVP;\n"
	"in vec4 vPos;\n"
	"in vec4 vNormal;\n"
	"in vec2 vTexCoord;\n"
	"out vec4 pos_raw;"
	"out vec4 normal_raw;\n"
	"out vec2 texCoord;\n"
	"void main()\n"
	"{\n"
	"    gl_Position = MVP * vPos;\n"
	"    pos_raw = vPos;"
	"    normal_raw = vNormal;\n"
	"    texCoord = vTexCoord;\n"
	"}\n";

const char* ShaderInterface::fragmentShaderText =
	"#version 150\n"
	"uniform vec4 light_pos;\n"
	"in vec4 pos_raw;\n"
	"in vec4 normal_raw;\n"
	"in vec2 texCoord;\n"
	"out vec4 fragColor;"
	"void main()\n"
	"{\n"
	"    vec4 pos = pos_raw / sqrt(pos_raw.w * pos_raw.w - dot(pos_raw.xyz, pos_raw.xyz));\n"
	"    vec4 normal = normal_raw / sqrt(-normal_raw.w * normal_raw.w + dot(normal_raw.xyz, normal_raw.xyz));\n"
	"    vec4 light_dir_raw = light_pos + pos * (-light_pos.w * pos.w + dot(light_pos.xyz, pos.xyz));\n"
	"    vec4 light_dir = light_dir_raw / sqrt(-light_dir_raw.w * light_dir_raw.w + dot(light_dir_raw.xyz, light_dir_raw.xyz));\n"
	"    float directness = -light_dir.w * normal.w + dot(light_dir.xyz, normal.xyz);\n"
	"    fragColor = vec4(vec3(mod(floor(texCoord * 10), 2) * 0.5 + 0.5, 0.0) * directness, 1.0);\n"
	"}\n";
