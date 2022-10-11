#pragma once
#include "Test.h"
#include "Shader.h"
#include "Camera.h"

#include <glm/glm.hpp>

namespace test
{
	class TestDepthTesting : public Test
	{
	public:
		TestDepthTesting();
		~TestDepthTesting();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
		void OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset) override;
	private:
		Shader* m_Shader;
		std::unique_ptr<Camera> m_Camera;
		GLuint m_CubeVAO, m_CubeVBO;
		GLuint m_PlaneVAO, m_PlaneVBO;
		GLuint m_CubeTextureID, m_PlaneTextureID;
		glm::mat4 m_Model, m_View, m_Proj;
		glm::vec3 m_ViewPos;
	};
}
