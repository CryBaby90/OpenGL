#pragma once
#include "Test.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include <glm/glm.hpp>

namespace test
{
	class TestAntiAliasing : public Test
	{
	public:
		TestAntiAliasing();
		~TestAntiAliasing();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
		void OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset) override;
		void OnProcessInput(GLFWwindow* window, GLfloat deltaTime) override;

	protected:
		int LoadImage(char const* filename) override;
		
	private:
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Shader> m_ScreenShader;
		std::unique_ptr<Camera> m_Camera;
		GLuint m_CubeVAO, m_CubeVBO;
		GLuint m_QuadVAO, m_QuadVBO;
		GLuint m_FBO, m_RBO;
		GLuint m_IntermediateFBO, m_TextureColorbuffer;
		glm::mat4 m_Model, m_View, m_Proj;
	};
}
