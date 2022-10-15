#pragma once
#include "Test.h"
#include "Shader.h"
#include "Camera.h"

#include <glm/glm.hpp>

namespace test
{
	class TestFaceCulling : public Test
	{
	public:
		TestFaceCulling();
		~TestFaceCulling();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
		void OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset) override;
		void LoadImage(GLuint* textureID, char const* filename) override;
	private:
		Shader* m_Shader;
		std::unique_ptr<Camera> m_Camera;
		GLuint m_VAO, m_VBO, m_EBO;
		GLuint m_CubeTextureID;
		glm::mat4 m_Model, m_View, m_Proj;
		glm::vec3 m_ViewPos;
	};
}
