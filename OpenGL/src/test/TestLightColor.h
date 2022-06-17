#pragma once
#include "Test.h"
#include "Shader.h"
#include "Camera.h"

#include <glm/glm.hpp>

namespace test
{
	class TestLightColor : public Test
	{
	public:
		TestLightColor();
		~TestLightColor();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	private:
		std::unique_ptr<Shader> m_LightingShader;
		std::unique_ptr<Shader> m_CubeShader;
		std::unique_ptr<Camera> m_Camera;
		GLuint m_CubeVAO, m_VBO, m_lightVAO;
		glm::mat4 m_Model, m_View, m_Proj;
	};
}
