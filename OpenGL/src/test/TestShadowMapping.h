#pragma once
#include "Test.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include <glm/glm.hpp>

namespace test
{
	class TestShadowMapping : public Test
	{
	public:
		TestShadowMapping();
		~TestShadowMapping();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
		void OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset) override;
		void OnProcessInput(GLFWwindow* window, GLfloat deltaTime) override;

	protected:
		int LoadImage(char const* filename) override;
		
	private:
		std::unique_ptr<Shader> m_DepthMapShader;
		std::unique_ptr<Shader> m_QuadShader;
		std::unique_ptr<Shader> m_SimpleDepthShader;
		std::unique_ptr<Camera> m_Camera;
		GLuint m_FloorTexture;
		GLuint m_PlaneVAO, m_PlaneVBO;
		GLuint m_CubeVAO, m_CubeVBO;
		GLuint m_QuadVAO, m_QuadVBO;
		GLuint m_DepthMapFBO, m_DepthMap;
		glm::mat4 m_Model, m_View, m_Proj;
		glm::mat4 m_LightSpaceMatrix;

		glm::vec3 m_LightPos;

		bool m_GammaEnabled = false;
		bool m_GammaKeyPressed = false;
		const GLuint SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
		const GLfloat near_plane = 1.0f, far_plane = 7.5f;

		int LoadImage(char const* filename, bool gammaCorrection);

		void RenderScene(Shader& shader);
		void RenderCube();
		void RenderQuad();
	};
}
