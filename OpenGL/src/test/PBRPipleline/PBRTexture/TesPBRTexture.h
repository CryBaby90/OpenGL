#pragma once
#include "../test/Test.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include <glm/glm.hpp>

namespace test
{
	class TesPBRTexture : public Test
	{
	public:
		TesPBRTexture();
		~TesPBRTexture();

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
		std::unique_ptr<Model> m_RealModel;

		GLuint m_AlbedoTexture, m_NormalTexture, m_MetallicTexture, m_RoughnessTexture, m_AoTexture;

		glm::vec3 lightPositions[1] = {
			glm::vec3(0.0f, 0.0f, 10.0f),
		};
		glm::vec3 lightColors[1] = {
			glm::vec3(150.0f, 150.0f, 150.0f),
		};
		int nrRows = 7;
		int nrColumns = 7;
		float spacing = 2.5;
		glm::mat4 m_Model, m_View, m_Proj;

		GLuint m_QuadVAO = 0;
		GLuint m_QuadVBO;
		GLuint m_CubeVAO = 0;
		GLuint m_CubeVBO = 0;
		GLuint m_SphereVAO = 0;
		unsigned int indexCount;
		int LoadImage(char const* filename, bool gammaCorrection);
		void RenderQuad();
		void RenderCube();
		void RenderSphere();
	};
}
