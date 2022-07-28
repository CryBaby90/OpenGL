#pragma once
#include "Test.h"
#include "Shader.h"
#include "Camera.h"
#include "Model.h"

#include <glm/glm.hpp>

namespace test
{
	class TestModel : public Test
	{
	public:
		TestModel();
		~TestModel();

		void OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset) override;
		void OnScroll(GLfloat xoffset, GLfloat yoffset) override;
		void OnProcessInput(GLFWwindow* window, GLfloat deltaTime) override;
		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	private:
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Camera> m_Camera;
		std::unique_ptr<Model> m_RealModel;
		glm::mat4 m_Model, m_View, m_Proj, m_Normal;
	};
}
