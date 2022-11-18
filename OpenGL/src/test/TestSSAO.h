#pragma once
#include "Test.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include <glm/glm.hpp>

namespace test
{
	class TestSSAO : public Test
	{
	public:
		TestSSAO();
		~TestSSAO();

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
		std::unique_ptr<Shader> m_SSAOShader;
		std::unique_ptr<Shader> m_SSAOBlurShader;
		std::unique_ptr<Camera> m_Camera;
		std::unique_ptr<Model> m_RealModel;

		glm::vec3 m_LightPos = glm::vec3(2.0, 4.0, -2.0);
		glm::vec3 m_LightColor = glm::vec3(0.2, 0.2, 0.7);
		glm::mat4 m_Model, m_View, m_Proj;

		GLuint m_QuadVAO = 0;
		GLuint m_QuadVBO;
		GLuint m_CubeVAO = 0;
		GLuint m_CubeVBO = 0;
		GLuint m_ssaoFBO, m_ssaoBlurFBO;
		GLuint m_ssaoColorBuffer, m_ssaoColorBufferBlur;
		GLuint m_NoiseTexture;

		std::vector<glm::vec3> m_ssaoKernel;

		GLuint m_gBuffer, m_gPositionDepth, m_gNormal, m_gAlbedoSpec, m_rboDepth;
		GLuint m_Attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };

		int LoadImage(char const* filename, bool gammaCorrection);
		void RenderQuad();
		void RenderCube();
		float ourLerp(float a, float b, float f);
	};
}
