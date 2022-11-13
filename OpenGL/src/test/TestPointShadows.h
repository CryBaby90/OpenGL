#pragma once
#include "Test.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include <glm/glm.hpp>

namespace test
{
	class TestPointShadows : public Test
	{
	public:
		TestPointShadows();
		~TestPointShadows();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
		void OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset) override;
		void OnProcessInput(GLFWwindow* window, GLfloat deltaTime) override;

	protected:
		int LoadImage(char const* filename) override;
		
	private:
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Shader> m_SimpleDepthShader;
		std::unique_ptr<Camera> m_Camera;
		GLuint m_FloorTexture;
		GLuint m_CubeVAO, m_CubeVBO;
		GLuint m_DepthMapFBO, m_DepthCubemap;
		glm::mat4 m_Model, m_View, m_Proj;

		glm::vec3 m_LightPos;

		bool shadows = true;
		bool shadowsKeyPressed = false;
		const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
		const GLfloat near_plane = 1.0f, far_plane = 25.0f;

		int LoadImage(char const* filename, bool gammaCorrection);

		void RenderScene(Shader& shader);
		void RenderCube();
	};
}
