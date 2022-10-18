#pragma once
#include "Test.h"
#include "Shader.h"
#include "Camera.h"

#include <glm/glm.hpp>

namespace test
{
	class TestCubemaps : public Test
	{
	public:
		TestCubemaps();
		~TestCubemaps();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
		void OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset) override;
		void LoadImage(GLuint* textureID, char const* filename) override;
		
	private:
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Shader> m_CubemapsShader;
		std::unique_ptr<Camera> m_Camera;
		GLuint m_CubeVAO, m_CubeVBO;
		GLuint m_CubemapsVAO, m_CubemapsVBO;
		//GLuint m_QuadVAO, m_QuadVBO;
		GLuint m_CubeTextureID, m_CubemapTextureID;
		glm::mat4 m_Model, m_View, m_Proj;
		glm::vec3 m_ViewPos;

	private:
		int LoadCubemap(std::vector<std::string> faces);
	};
}
