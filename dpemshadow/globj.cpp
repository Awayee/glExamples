#include "globj.h"

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <vector>
#include <glm\glm.hpp>

#include <iostream>
#include <fstream>
#include <sstream>

namespace globj {
	void CheckShaderCompileError(uint32_t shader, std::string type) {
		int success;
		char infoLog[1024];
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}

	std::string ReadFile(const char* path) {
		std::ifstream file;
		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		file.open(path);
		std::stringstream contentStream;
		contentStream << file.rdbuf();
		file.close();
		char* content = NULL;
		return contentStream.str();
	}

	uint32_t CreateShader(const char* path, uint32_t type) {
		// ¶ÁÎÄ¼þ
		std::ifstream file;
		file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		file.open(path);
		std::stringstream contentStream;
		contentStream << file.rdbuf();
		file.close();
		std::string sCode = contentStream.str();

		const char* code = sCode.c_str();
		uint32_t shader = glCreateShader(type);
		glShaderSource(shader, 1, &code, NULL);
		glCompileShader(shader);
		std::string sType;
		if (type == GL_VERTEX_SHADER) {
			sType = "VERTEX";
		}
		else if (type == GL_FRAGMENT_SHADER) {
			sType = "FRAGMENT";
		}
		else if (type == GL_GEOMETRY_SHADER) {
			sType = "GEOMETRY";
		}
		CheckShaderCompileError(shader, sType);
		return shader;
	}

	void Shader::Init(const char* vertexPath, const char* fragmentPath, const char* geometryPath) {
		uint32_t vertex, fragment, geometry;
		m_ID = glCreateProgram();
		if (vertexPath) {
			vertex = CreateShader(vertexPath, GL_VERTEX_SHADER);
			glAttachShader(m_ID, vertex);
		}
		if (fragmentPath) {
			fragment = CreateShader(fragmentPath, GL_FRAGMENT_SHADER);
			glAttachShader(m_ID, fragment);
		}
		if (geometryPath) {
			geometry = CreateShader(geometryPath, GL_GEOMETRY_SHADER);
			glAttachShader(m_ID, geometry);
		}
		glLinkProgram(m_ID);
		if (vertexPath)glDeleteShader(vertex);
		if (fragmentPath)glDeleteShader(fragment);
		if (geometryPath)glDeleteShader(geometry);
	}
}