#pragma once
#include "Test.h"
#include "Shader.h"

#include <map>

namespace test
{
	struct Character {
		GLuint TextureID;   // ID handle of the glyph texture
		glm::ivec2 Size;    // Size of glyph
		glm::ivec2 Bearing;  // Offset from baseline to left/top of glyph
		GLuint Advance;    // Horizontal offset to advance to next glyph
	};

	class TestTextRendering : public Test
	{
	public:
		TestTextRendering();
		~TestTextRendering();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	private:
		Shader* m_Shader;
		GLuint m_VAO, m_VBO;

		std::map<GLchar, Character> Characters;

		void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color);
	};
}
