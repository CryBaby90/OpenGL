#pragma once
#include "Test.h"
#include "Shader.h"

#include <glm/glm.hpp>

namespace test
{
	class TestCoordinate : public Test
	{
	public:
		TestCoordinate();
		~TestCoordinate();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	private:
		Shader* m_Shader;
		GLuint m_VAO, m_VBO, m_EBO;
		GLuint m_TextureID1, m_TextureID2;
		GLfloat m_MixValue;
		glm::mat4 m_Model, m_View, m_Proj;
		glm::vec3* m_CubePositions;
		glm::vec3 m_ViewPos;
	};
}
