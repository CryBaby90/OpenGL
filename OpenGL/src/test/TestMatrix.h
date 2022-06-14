#pragma once
#include "Test.h"
#include "Shader.h"

#include <glm/glm.hpp>

namespace test
{
	class TestMatrix : public Test
	{
	public:
		TestMatrix();
		~TestMatrix();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	private:
		Shader* m_Shader;
		GLuint m_VAO, m_VBO, m_EBO;
		GLuint m_TextureID1, m_TextureID2;
		GLfloat m_MixValue;
		glm::mat4 m_Trans;
		GLfloat m_Rotate;
		glm::vec3 m_Scale;
	};
}
