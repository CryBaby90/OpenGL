#pragma once
#include "Test.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include <glm/glm.hpp>

namespace test
{
	class TestParallaxMapping : public Test
	{
	public:
		TestParallaxMapping();
		~TestParallaxMapping();

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
		GLuint m_DiffuseMap;
		GLuint m_NormalMap;
		GLuint m_HeightMap;
		GLuint m_QuadVAO = 0;
		GLuint m_QuadVBO;
		glm::mat4 m_Model, m_View, m_Proj;

		float heightScale = 0.1f;

		glm::vec3 m_LightPos;

		int LoadImage(char const* filename, bool gammaCorrection);
		void RenderQuad();
	};
}
