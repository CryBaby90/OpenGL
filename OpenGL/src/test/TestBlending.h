#pragma once
#include "Test.h"
#include "Shader.h"
#include "Camera.h"

#include <glm/glm.hpp>

namespace test
{
	class TestBlending : public Test
	{
	public:
		TestBlending();
		~TestBlending();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
		void OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset) override;
		void LoadImage(GLuint* textureID, char const* filename) override;
	private:
		Shader* m_Shader;
		Shader* m_SingleColorShader;
		std::unique_ptr<Camera> m_Camera;
		GLuint m_CubeVAO, m_CubeVBO;
		GLuint m_PlaneVAO, m_PlaneVBO;
		GLuint m_TransparenVAO, m_TransparenVBO;
		GLuint m_CubeTextureID, m_PlaneTextureID, m_TransparenTextureID;
		glm::mat4 m_Model, m_View, m_Proj;
		glm::vec3 m_ViewPos;
	};
}
