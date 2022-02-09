#pragma once
#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <vector>
#include <glm\glm.hpp>
#include "globj.h"

namespace glapp {
	// 绘制三角形

	class GLApp {
	public:
		void InitWindow();
		void InitGL();
		void Run();
		void Clear();
	private:
		GLFWwindow* m_pWindow = NULL;
		uint32_t m_Texture1;
		uint32_t m_Texture2;
		globj::Shader m_CubeShader;
		globj::Shader m_LightShader;
		globj::Shader m_DpemShader;
		globj::Camera m_Camera;
		globj::PointLight m_PointLight;
		std::vector<globj::Cube> m_Cubes;
		uint32_t CreateTexture(const char* file, uint32_t format); // 创建纹理
		void DrawFrame();
		// 相机
		void CreateCamera(globj::Camera* pCamera);
		// 点光源
		void CreatePointLight(globj::PointLight* pLight);
		void RenderPointLight(globj::PointLight* pLight, globj::Shader* pShader);
		void DestroyPointLight(globj::PointLight* pLight);
		// 阴影
		void CreateDepthMap(globj::DepthMap* pMap);
		void RenderDepthMap(globj::DepthMap& map, globj::Shader& shader, bool front=true);
		// 立方体
		void InitResources();
		void CreateCube(globj::Cube* pCube);
		void RenderCube(globj::Cube* pCube, globj::Shader* pShader);
		void DestroyCube(globj::Cube* pCube);
	};
}
