#pragma once
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <vector>
#include <glm\glm.hpp>
#include <glm\gtc\matrix_transform.hpp>

namespace globj {
	struct Shader {
		uint32_t m_ID;
		Shader() { m_ID = 0; };
		Shader(const char* vertexPath, const char* fragmentPath, const char* geometryPath) { Init(vertexPath, fragmentPath, geometryPath); };
		void Init(const char* vertexPath, const char* fragmentPath, const char* geometryPath);
		void Use() {
			glUseProgram(m_ID);
		}
		void Clear() {
			if (m_ID)glDeleteProgram(m_ID);
		}

		void SetBool(const char* name, bool value) {
			glUniform1i(glGetUniformLocation(m_ID, name), value);
		}

		void SetInt(const char* name, int value) {
			glUniform1i(glGetUniformLocation(m_ID, name), value);
		}

		void SetFloat(const char* name, float value) {
			glUniform1f(glGetUniformLocation(m_ID, name), value);
		}

		void SetVec2(const char* name, glm::vec2& value) {
			glUniform2fv(glGetUniformLocation(m_ID, name), 1, &value[0]);
		}

		void SetVec3(const char* name, glm::vec3& value) {
			glUniform3fv(glGetUniformLocation(m_ID, name), 1, &value[0]);
		}

		void SetVec4(const char* name, glm::vec4& value) {
			glUniform4fv(glGetUniformLocation(m_ID, name), 1, &value[0]);
		}

		void SetMat2(const char* name, const glm::mat2& mat) const
		{
			glUniformMatrix2fv(glGetUniformLocation(m_ID, name), 1, GL_FALSE, &mat[0][0]);
		}

		void SetMat3(const char* name, const glm::mat3& mat) const
		{
			glUniformMatrix3fv(glGetUniformLocation(m_ID, name), 1, GL_FALSE, &mat[0][0]);
		}

		void SetMat4(const char* name, const glm::mat4& mat) const
		{
			glUniformMatrix4fv(glGetUniformLocation(m_ID, name), 1, GL_FALSE, &mat[0][0]);
		}

	};

	struct Camera {
		// transform
		glm::vec3 pos;
		glm::vec3 at;
		glm::vec3 up;
		// perspective
		float fov;
		float aspect;
		float nearClip;
		float farClip;
		// matrix
		glm::mat4 viewMat;
		glm::mat4 projMat;

		void SetTransform(glm::vec3 camPos, glm::vec3 camAt, glm::vec3 camUp) {
			pos = camPos;
			at = camAt;
			up = camUp;
			viewMat = glm::lookAt(pos, at, up);
		}

		void SetPerspective(float camFov, float camAspect, float camNear, float camFar){
			fov = camFov;
			aspect = camAspect;
			nearClip = camNear;
			farClip = camFar;
			projMat = glm::perspective(fov, aspect, nearClip, farClip);
		}

	};

	struct RenderTexture {
		uint32_t w = 512, h = 512;
		uint32_t frameBuffer;
		uint32_t texture;
	};

	struct PointLight {
		std::vector<float> vertices = {
			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,

			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f, -0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,

			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,

			-0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f, -0.5f,
			 0.5f, -0.5f,  0.5f,
			 0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f,  0.5f,
			-0.5f, -0.5f, -0.5f,

			-0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f, -0.5f,
			 0.5f,  0.5f,  0.5f,
			 0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f,  0.5f,
			-0.5f,  0.5f, -0.5f,
		};
		uint32_t vertexBuffer, vertexArray;
		glm::vec3 color;
		glm::vec3 pos;
		float strength;
		glm::mat4 transformMat;
		//Ë«Å×ÃæÒõÓ°Ó³Éä
		RenderTexture frontMap;
		RenderTexture backMap;
		float fov = 1.576f, aspect = 1.0f, near = 0.1f, far = 100.0f;
		void SetPos(glm::vec3 vPos) {
			pos = vPos;
			transformMat = glm::scale(glm::rotate(glm::translate(glm::mat4(1.0f), pos), glm::radians(45.0f), { 0.0f, 1.0f, 1.0f }), { 0.1f, 0.1f, 0.1f });
		}
	};

	struct Cube {
		std::vector<float> vertices;
		uint32_t vertexBuffer, vertexArray;
		glm::mat4 transformMat;
		glm::vec3 color;
	};
}
