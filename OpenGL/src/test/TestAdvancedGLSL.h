#pragma once
#include "Test.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include <glm/glm.hpp>

namespace test
{
	class TestAdvancedGLSL : public Test
	{
	public:
		TestAdvancedGLSL();
		~TestAdvancedGLSL();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
		void OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset) override;
		void OnProcessInput(GLFWwindow* window, GLfloat deltaTime) override;

	protected:
		int LoadImage(char const* filename) override;
		
	private:
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Shader> m_CubemapsShader;
		std::unique_ptr<Camera> m_Camera;
		GLuint m_CubeVAO, m_CubeVBO;
		GLuint m_CubemapsVAO, m_CubemapsVBO;
		GLuint m_CubeTextureID, m_CubemapTextureID;
		GLuint m_UboMatrices;
		glm::mat4 m_Model, m_View, m_Proj;
		glm::vec3 m_ViewPos;

	private:
		int LoadCubemap(std::vector<std::string> faces);
	};
}
