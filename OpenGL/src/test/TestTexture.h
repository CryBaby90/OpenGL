#pragma once
#include "Test.h"
#include "Shader.h"

namespace test
{
	class TestTexture : public Test
	{
	public:
		TestTexture();
		~TestTexture();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	private:
		Shader* m_Shader;
		GLuint m_VAO, m_VBO, m_EBO;
		GLuint m_TextureID1, m_TextureID2;
	};
}
