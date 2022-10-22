#pragma once
#include "Test.h"
#include "Shader.h"
#include "Camera.h"

#include <glm/glm.hpp>

namespace test
{
	class TestFramebuffers : public Test
	{
	public:
		TestFramebuffers();
		~TestFramebuffers();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
		void OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset) override;

	protected:
		int LoadImage(char const* filename) override;

	private:
		Shader* m_Shader;
		Shader* m_ScreenShader;
		std::unique_ptr<Camera> m_Camera;
		GLuint m_CubeVAO, m_CubeVBO;
		GLuint m_PlaneVAO, m_PlaneVBO;
		GLuint m_QuadVAO, m_QuadVBO;
		GLuint m_FBO, m_RBO;
		GLuint m_CubeTextureID, m_FloorTextureID, m_TextureColorbuffer;
		glm::mat4 m_Model, m_View, m_Proj;
		glm::vec3 m_ViewPos;
	};
}
