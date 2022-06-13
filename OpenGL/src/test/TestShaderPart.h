#pragma once
#include "Test.h"
#include "Shader.h"

namespace test
{
	class TestShaderPart : public Test
	{
	public:
		TestShaderPart();
		~TestShaderPart();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	private:
		Shader* m_Shader;
		GLuint m_VAO, m_VBO, m_EBO;
	};
}
