#pragma once
#include "Test.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include <glm/glm.hpp>

namespace test
{
	class TestPlanetInstancing : public Test
	{
	public:
		TestPlanetInstancing();
		~TestPlanetInstancing();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
		void OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset) override;
		void OnProcessInput(GLFWwindow* window, GLfloat deltaTime) override;

	protected:
		int LoadImage(char const* filename) override;
		
	private:
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Shader> m_InstanceShader;
		std::unique_ptr<Camera> m_Camera;
		std::unique_ptr<Model> m_Planet;
		std::unique_ptr<Model> m_Rock;
		glm::mat4* m_ModelMatrices;
		GLuint m_InstanceVBO;
		unsigned int m_Amount = 20000;

	private:
		int LoadCubemap(std::vector<std::string> faces);
		void InitModelMatrices();
	};
}
