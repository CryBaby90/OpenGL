#pragma once
#include "Test.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include <glm/glm.hpp>

namespace test
{
	class TestGammaCorrection : public Test
	{
	public:
		TestGammaCorrection();
		~TestGammaCorrection();

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
		GLuint m_FloorTexture;
		GLuint m_FloorTextureGammaCorrected;
		GLuint m_CubeVAO, m_CubeVBO;
		glm::mat4 m_Model, m_View, m_Proj;
		glm::vec3 m_LightPositions[4] = {
			glm::vec3(-3.0f, 0.0f, 0.0f),
			glm::vec3(-1.0f, 0.0f, 0.0f),
			glm::vec3(1.0f, 0.0f, 0.0f),
			glm::vec3(3.0f, 0.0f, 0.0f)
		};;
		glm::vec3 m_LightColors[4] = {
			glm::vec3(0.25),
			glm::vec3(0.50),
			glm::vec3(0.75),
			glm::vec3(1.00)
		};;
		bool m_GammaEnabled = false;
		bool m_GammaKeyPressed = false;

		int LoadImage(char const* filename, bool gammaCorrection);
	};
}
