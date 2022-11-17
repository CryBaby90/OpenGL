#pragma once
#include "Test.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include <glm/glm.hpp>

namespace test
{
	class TestDeferredShading : public Test
	{
	public:
		TestDeferredShading();
		~TestDeferredShading();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
		void OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset) override;
		void OnProcessInput(GLFWwindow* window, GLfloat deltaTime) override;

	protected:
		int LoadImage(char const* filename) override;
		
	private:
		std::unique_ptr<Shader> m_GBufferShader;
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Camera> m_Camera;
		std::unique_ptr<Model> m_RealModel;

		std::vector<glm::vec3> m_ObjectPositions;
		std::vector<glm::vec3> m_LightPositions;
		std::vector<glm::vec3> m_LightColors;
		glm::mat4 m_Model, m_View, m_Proj;

		unsigned int quadVAO = 0;
		unsigned int quadVBO;

		GLuint m_gBuffer, m_gPosition, m_gNormal, m_gAlbedoSpec, m_rboDepth;
		GLuint m_Attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

		int LoadImage(char const* filename, bool gammaCorrection);
		void RenderQuad();
	};
}
