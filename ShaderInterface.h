#pragma once
#include "glad.h"
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
		lightPosLocation = glGetUniformLocation(program, "light");
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
	"out vec4 pos;"
	"out vec4 normal;\n"
	"out vec2 texCoord;\n"
	"void main()\n"
	"{\n"
	"    gl_Position = MVP * vPos;\n"
	"    pos = vPos;"
	"    normal = vNormal;\n"
	"    texCoord = vTexCoord;\n"
	"}\n";

const char* ShaderInterface::fragmentShaderText =
	"#version 150\n"
	"uniform sampler2D texture_sampler;\n"
	"uniform vec4 light;\n"
	"in vec4 pos;\n"
	"in vec4 normal;\n"
	"in vec2 texCoord;\n"
	"out vec4 fragColor;\n"
	"float hypdot(vec4 v1, vec4 v2)\n"
	"{\n"
	"    return dot(v1.xyz, v2.xyz) - v1.w * v2.w;\n"
	"}\n"
	"void main()\n"
	"{\n"
	"    float hypdot_pos_pos = hypdot(pos, pos);\n"
	"    float hypdot_pos_light = hypdot(pos, light);\n"
	"    float hypdot_pos_normal = hypdot(pos, normal);\n"
	"    float numerator = hypdot(light, normal) * hypdot_pos_pos - hypdot_pos_light * hypdot_pos_normal;\n"
	"    float denominator_light = hypdot(light, light) * hypdot_pos_pos - hypdot_pos_light * hypdot_pos_light;\n"
	"    float denominator_normal = hypdot(normal, normal) * hypdot_pos_pos - hypdot_pos_normal * hypdot_pos_normal;\n"
	"    float directness = max(0.0, -(gl_FrontFacing ? 1.0 : -1.0) * numerator / sqrt(denominator_light * denominator_normal));\n"
	"    fragColor = vec4(texture(texture_sampler, texCoord).rgb * directness, 1.0);\n"
	"}\n";
