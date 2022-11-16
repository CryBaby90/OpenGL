#pragma once
#include "Test.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include <glm/glm.hpp>

namespace test
{
	class TestBloom : public Test
	{
	public:
		TestBloom();
		~TestBloom();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
		void OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset) override;
		void OnProcessInput(GLFWwindow* window, GLfloat deltaTime) override;

	protected:
		int LoadImage(char const* filename) override;
		
	private:
		std::unique_ptr<Shader> m_BloomShader;
		std::unique_ptr<Shader> m_LightShader;
		std::unique_ptr<Shader> m_BlurShader; //模糊处理
		std::unique_ptr<Shader> m_FinalShader;

		std::unique_ptr<Camera> m_Camera;
		GLuint m_WoodTexture, m_ContainerTexture;
		GLuint m_ColorBuffers[2];

		GLuint m_QuadVAO = 0;
		GLuint m_QuadVBO;
		GLuint m_CubeVAO = 0;
		GLuint m_CubeVBO = 0;
		GLuint m_HdrFBO, m_DepthRBO;
		GLuint m_Attachments[2]{ GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 }; //颜色附件，分别对应两个帧缓冲
		GLuint m_PingpongFBO[2], m_PingpongColorbuffers[2]; //高斯模糊用

		glm::mat4 m_Model, m_View, m_Proj;

		bool bloom = true;
		bool bloomKeyPressed = false;
		float exposure = 1.0f;

		std::vector<glm::vec3> m_LightPositions;
		std::vector<glm::vec3> m_LightColors;

		int LoadImage(char const* filename, bool gammaCorrection);
		void RenderQuad();
		void RenderCube();
	};
}
