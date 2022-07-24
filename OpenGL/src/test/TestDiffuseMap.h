#pragma once
#include "Test.h"
#include "Shader.h"
#include "Camera.h"

#include <glm/glm.hpp>

namespace test
{
	class TestDiffuseMap : public Test
	{
	public:
		TestDiffuseMap();
		~TestDiffuseMap();

		void OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset) override;
		void OnScroll(GLfloat xoffset, GLfloat yoffset) override;
		void OnProcessInput(GLFWwindow* window, GLfloat deltaTime) override;
		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	private:
		unsigned int loadTexture(char const* path);
	private:
		std::unique_ptr<Shader> m_LightingShader;
		std::unique_ptr<Shader> m_CubeShader;
		std::unique_ptr<Camera> m_Camera;
		GLuint m_CubeVAO, m_VBO, m_lightVAO, m_DiffuseMapID, m_SpecularMapID;
		glm::mat4 m_Model, m_View, m_Proj, m_Normal;
		glm::vec3 m_LightPos;// lighting
	};
}
