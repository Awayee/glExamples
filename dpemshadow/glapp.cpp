#include<glad\glad.h>

#include<GLFW\glfw3.h>
#include<glm\glm.hpp>
#include<glm\gtc\matrix_transform.hpp>
#include <iostream>
#include <string>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include "glapp.h"


namespace glapp {

	const uint32_t SCR_WIDTH = 800;
	const uint32_t SCR_HEIGHT = 600;

	// 窗口尺寸变换
	void FrameBufferSizeCallback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
	}

	// 处理输入
	void KeyCallback(GLFWwindow* window, int key, int scan, int action, int mod) {
		if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
			glfwSetWindowShouldClose(window, true);
		}
	}

	void GLApp::InitWindow() {
		glfwInit();
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		m_pWindow = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "OpenGL", nullptr, nullptr);
		glfwSetFramebufferSizeCallback(m_pWindow, FrameBufferSizeCallback);
		glfwSetKeyCallback(m_pWindow, KeyCallback);
		if (m_pWindow == nullptr)
		{
			std::cout << "Failed to create GLFW window" << std::endl;
			glfwTerminate();
			return;
		}
		glfwMakeContextCurrent(m_pWindow);
	}

	void GLApp::InitGL() {
		InitResources();
		if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		{
			std::cout << "Failed to initialize GLAD" << std::endl;
			return;
		}
		// 视口
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		CreateCamera(&m_Camera);
		CreatePointLight(&m_PointLight);
		for (auto& cube : m_Cubes) {
			CreateCube(&cube);
		}
		// 开启深度
		glEnable(GL_DEPTH_TEST);
		// 纹理
		m_Texture1 = CreateTexture("textures/container.jpg", GL_RGB);
		m_Texture2 = CreateTexture("textures/awesomeface.png", GL_RGBA); 
		// 初始化Shader
		m_CubeShader.Init("shaders/cube.vert", "shaders/cube.frag", nullptr);
		m_CubeShader.Use();

		m_LightShader.Init("shaders/light.vert", "shaders/light.frag", nullptr);

		m_DpemShader.Init("shaders/dpemshadow.vert", nullptr, nullptr);
	}

	void GLApp::DrawFrame() {
		float time = glfwGetTime();
		// 转起来
		//m_Cubes[0].transformMat = glm::translate(glm::mat4(1.0f), { 3.0 * glm::sin(time) * 1.57, 2.0f, 0.0f });
		m_DpemShader.Use();
		m_DpemShader.SetVec3("lightPos", m_PointLight.pos);
		m_DpemShader.SetFloat("near", m_PointLight.near);
		m_DpemShader.SetFloat("far", m_PointLight.far);
		DrawRenderTexture(m_PointLight.frontMap, m_DpemShader, true);
		DrawRenderTexture(m_PointLight.backMap, m_DpemShader, false);
		glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		RenderPointLight(&m_PointLight, &m_LightShader);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, m_Texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, m_Texture2);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, m_PointLight.frontMap.texture);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, m_PointLight.backMap.texture);
		for (auto& cube : m_Cubes) {
			RenderCube(&cube, &m_CubeShader);
		}
	}

	void GLApp::Run() {
		while (!glfwWindowShouldClose(m_pWindow)){
			DrawFrame();
			glfwSwapBuffers(m_pWindow);
			glfwPollEvents();
		}
	}

	void GLApp::Clear() {
		for (auto& cube : m_Cubes) {
			DestroyCube(&cube);
		}
		m_CubeShader.Clear();
		m_DpemShader.Clear();
		m_LightShader.Clear();
		glfwTerminate();
	}

	uint32_t GLApp::CreateTexture(const char* file, uint32_t format) {
		uint32_t texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		//设置纹理参数
		// 为当前绑定的纹理对象设置环绕、过滤方式
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		//stbi_set_flip_vertically_on_load(true);
		int width, height, channels;
		unsigned char* data = stbi_load(file, &width, &height, &channels, 0);
		if (data) {
			glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		else {
			std::cout << "failed to load texture: " << file << std::endl;
		}
		stbi_image_free(data); //释放
		return texture;

	}

	void GLApp::CreateCamera(globj::Camera* pCamera) {
		pCamera->SetTransform({ 10.0f, 20.0f, 16.0f }, { 0.0f, 3.0f, 0.0f }, { 0.0f, 1.0f, 0.0f });
		pCamera->SetPerspective(0.78, SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
	}


	void GLApp::CreateRenderTexture(globj::RenderTexture* pTex) {
		//创建纹理
		glGenTextures(1, &pTex->texture);
		glBindTexture(GL_TEXTURE_2D, pTex->texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, pTex->w, pTex->h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		//绑定帧缓冲
		glGenFramebuffers(1, &pTex->frameBuffer);
		glBindFramebuffer(GL_FRAMEBUFFER, pTex->frameBuffer);
		glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, pTex->texture, 0);
		glEnable(GL_DEPTH_TEST);
		glDrawBuffer(GL_NONE);
		glReadBuffer(GL_NONE);
		//重置
		glBindTexture(GL_TEXTURE_2D, 0);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void GLApp::DrawRenderTexture(globj::RenderTexture& tex, globj::Shader& shader, bool front) {
		glViewport(0, 0, tex.w, tex.h);
		glBindFramebuffer(GL_FRAMEBUFFER, tex.frameBuffer);
		glClear(GL_DEPTH_BUFFER_BIT);
		shader.Use();
		shader.SetFloat("dir", front ? 1.0f : -1.0f); // 方向
		for (auto& cube : m_Cubes) {
			shader.SetMat4("model", cube.transformMat);
			glBindVertexArray(cube.vertexArray);
			glDrawArrays(GL_TRIANGLES, 0, cube.vertices.size());
		}
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}

	void GLApp::DestroyRenderTexture(globj::RenderTexture* pTex) {
		glDeleteFramebuffers(1, &pTex->frameBuffer);
		glDeleteTextures(1, &pTex->texture);
	}

	void GLApp::CreatePointLight(globj::PointLight* pLight){
		glGenVertexArrays(1, &pLight->vertexArray);
		glGenBuffers(1, &pLight->vertexBuffer);
		glBindVertexArray(pLight->vertexArray);
		glBindBuffer(GL_ARRAY_BUFFER, pLight->vertexBuffer);
		glBufferData(GL_ARRAY_BUFFER, pLight->vertices.size() * sizeof(pLight->vertices[0]), pLight->vertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		pLight->SetPos({ 0.0f, 10.0f, 0.0f });
		pLight->color = { 0.71f, 0.7f, 0.6f };
		CreateRenderTexture(&pLight->frontMap);
		CreateRenderTexture(&pLight->backMap);
	}
	void GLApp::RenderPointLight(globj::PointLight* pLight, globj::Shader* pShader) {
		pShader->Use();
		pShader->SetVec3("lightColor", pLight->color);
		pShader->SetFloat("lightStrength", pLight->strength);
		pShader->SetMat4("model", pLight->transformMat);
		pShader->SetMat4("view", m_Camera.viewMat);
		pShader->SetMat4("proj", m_Camera.projMat);
		glBindVertexArray(pLight->vertexArray);
		glDrawArrays(GL_TRIANGLES, 0, pLight->vertices.size());
	}
	void GLApp::DestroyPointLight(globj::PointLight* pLight) {
		glDeleteVertexArrays(1, &pLight->vertexArray);
		glDeleteBuffers(1, &pLight->vertexBuffer);
		DestroyRenderTexture(&pLight->frontMap);
		DestroyRenderTexture(&pLight->backMap);
	}

	void GLApp::InitResources() {
		globj::Cube cube1{};
		cube1.vertices = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		cube1.transformMat = glm::translate(glm::mat4(1.0f), { 0.0f, 2.0f, 0.0f });
		globj::Cube cube2{};
		cube2.vertices = {
			// positions            // normals         // texcoords
			 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
			-25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
			-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,

			 25.0f, -0.5f,  25.0f,  0.0f, 1.0f, 0.0f,  25.0f,  0.0f,
			-25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,   0.0f, 25.0f,
			 25.0f, -0.5f, -25.0f,  0.0f, 1.0f, 0.0f,  25.0f, 25.0f
		};
		cube2.transformMat = glm::mat4(1.0f);
		m_Cubes.push_back(cube1);
		m_Cubes.push_back(cube2);
		globj::Cube cube3 = cube1;
		cube3.transformMat = glm::translate(glm::mat4(1.0f), { 0.0f, 2.0f, 4.0f });
		globj::Cube cube4 = cube1;
		cube4.transformMat = glm::translate(glm::mat4(1.0f), { 0.0f, 2.0f, -4.0f });
		globj::Cube cube5 = cube1;
		cube5.transformMat = glm::translate(glm::mat4(1.0f), { 4.0f, 2.0f, 0.0f });
		globj::Cube cube6 = cube1;
		cube6.transformMat = glm::translate(glm::mat4(1.0f), { -4.0f, 2.0f, 0.0f });

		m_Cubes.push_back(cube3);
		m_Cubes.push_back(cube4);
		m_Cubes.push_back(cube5);
		m_Cubes.push_back(cube6);
	}

	void GLApp::CreateCube(globj::Cube* pCube) {
		glGenVertexArrays(1, &pCube->vertexArray);
		glGenBuffers(1, &pCube->vertexBuffer);
		// 先绑定顶点数组，再绑定顶点缓冲，然后配置顶点属性
		glBindVertexArray(pCube->vertexArray);
		//	绑定顶点缓冲
		glBindBuffer(GL_ARRAY_BUFFER, pCube->vertexBuffer);
		//	GL_STATIC_DRAW：数据不会或几乎不会改变；GL_DYNAMIC_DRAW：数据会被改变很多；GL_STREAM_DRAW ：数据每次绘制时都会改变。
		glBufferData(GL_ARRAY_BUFFER, pCube->vertices.size() * sizeof(pCube->vertices[0]), pCube->vertices.data(), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glEnableVertexAttribArray(2);

		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
		pCube->color = { 0.7f, 0.8f, 0.9f };
	}

	void GLApp::RenderCube(globj::Cube* pCube, globj::Shader* pShader) {
		pShader->Use();
		pShader->SetMat4("model", pCube->transformMat);
		pShader->SetMat4("view", m_Camera.viewMat);
		pShader->SetMat4("proj", m_Camera.projMat);
		pShader->SetVec3("lightColor", m_PointLight.color);
		pShader->SetVec3("lightPos", m_PointLight.pos);
		pShader->SetVec3("viewPos", m_Camera.pos);
		pShader->SetFloat("lightNear", m_PointLight.near);
		pShader->SetFloat("lightFar", m_PointLight.far);
		pShader->SetInt("texture1", 0);
		pShader->SetInt("texture2", 1);
		pShader->SetInt("frontMap", 2);
		pShader->SetInt("backMap", 3);
		glBindVertexArray(pCube->vertexArray);
		glDrawArrays(GL_TRIANGLES, 0, pCube->vertices.size());
	}

	void GLApp::DestroyCube(globj::Cube* pCube) {
		glDeleteVertexArrays(1, &pCube->vertexArray);
		glDeleteBuffers(1, &pCube->vertexBuffer);
	}
}