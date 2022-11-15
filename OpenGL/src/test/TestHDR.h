#pragma once
#include "Test.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include <glm/glm.hpp>

namespace test
{
	class TestHDR : public Test
	{
	public:
		TestHDR();
		~TestHDR();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
		void OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset) override;
		void OnProcessInput(GLFWwindow* window, GLfloat deltaTime) override;

	protected:
		int LoadImage(char const* filename) override;
		
	private:
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Shader> m_HdrShader;
		std::unique_ptr<Camera> m_Camera;
		GLuint m_DiffuseMap;
		GLuint m_ColorBuffer;

		GLuint m_QuadVAO = 0;
		GLuint m_QuadVBO;
		GLuint m_CubeVAO = 0;
		GLuint m_CubeVBO = 0;
		GLuint m_HdrFBO;

		glm::mat4 m_Model, m_View, m_Proj;

		bool hdr = true;
		bool hdrKeyPressed = false;
		float exposure = 1.0f;

		std::vector<glm::vec3> m_LightPositions;
		std::vector<glm::vec3> m_LightColors;

		int LoadImage(char const* filename, bool gammaCorrection);
		void RenderQuad();
		void RenderCube();
	};
}
