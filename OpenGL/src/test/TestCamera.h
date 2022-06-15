#pragma once
#include "Test.h"
#include "Shader.h"

#include <glm/glm.hpp>

namespace test
{
	class TestCamera : public Test
	{
	public:
		TestCamera();
		~TestCamera();

		void OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true) override;
		void OnScroll(GLfloat xoffset, GLfloat yoffset) override;
		void OnProcessInput(GLFWwindow* window, GLfloat deltaTime) override;
		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	private:
		Shader* m_Shader;
		GLuint m_VAO, m_VBO, m_EBO;
		GLuint m_TextureID1, m_TextureID2;
		GLfloat m_MixValue, m_CameraSpeed;
		glm::mat4 m_Model, m_View, m_Proj;
		glm::vec3* m_CubePositions;
		glm::vec3 m_CameraPos, m_CameraFront, m_CameraUp;

		GLfloat Yaw = -90.0f;
		GLfloat Pitch = 0.0f;
		GLfloat SPEED = 2.5f;
		GLfloat ZOOM = 45.0f;
		GLfloat fov = 45.0f;
	};
}
