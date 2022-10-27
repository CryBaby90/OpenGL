#pragma once
#include "Test.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include <glm/glm.hpp>

namespace test
{
	class TestInstancing : public Test
	{
	public:
		TestInstancing();
		~TestInstancing();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
		void OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset) override;
		void OnProcessInput(GLFWwindow* window, GLfloat deltaTime) override;

	protected:
		int LoadImage(char const* filename) override;
		
	private:
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Camera> m_Camera;
		glm::vec2 m_Translations[100];
		GLuint m_CubeVAO, m_CubeVBO;
		GLuint m_InstanceVBO;

	private:
		int LoadCubemap(std::vector<std::string> faces);
	};
}
