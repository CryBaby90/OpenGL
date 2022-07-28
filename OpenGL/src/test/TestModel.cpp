#include "TestModel.h"
#include "Global.h"

#include <stb_image/stb_image.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>

test::TestModel::TestModel()
	:m_Shader(nullptr), m_Camera(nullptr), m_RealModel(nullptr),
	m_Model(glm::mat4(1.0f)), m_View(glm::mat4(1.0f)), m_Proj(glm::mat4(1.0f)), m_Normal(glm::mat3(1.0f))
{
	//开启z-buffer 见OpenGL 坐标系统
	//GLCall(glEnable(GL_DEPTH_TEST));

	m_Shader = std::make_unique<Shader>("res/shaders/Model/Vertex.Vshader", "res/shaders/Model/Fragement.Fshader");
	m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
	m_RealModel = std::make_unique<Model>("res/obj/nanosuit.obj");
}

test::TestModel::~TestModel()
{
	GLCall(glBindVertexArray(0));
}

void test::TestModel::OnUpdate(float deltaTime)
{
	m_View = m_Camera->GetViewMatrix();
	m_Proj = m_Camera->GetProjMatrix();
}

void test::TestModel::OnRender()
{
	m_Shader->Bind();
	m_Model = glm::mat4(1.0f);
	m_Model = glm::translate(m_Model, glm::vec3(0.0f, -0.5f, 0.0f));
	m_Model = glm::scale(m_Model, glm::vec3(0.1f, 0.1f, 0.1f));
	m_Shader->SetUniformsMat4f("model", m_Model);
	m_Shader->SetUniformsMat4f("view", m_View);
	m_Shader->SetUniformsMat4f("proj", m_Proj);
	m_RealModel->Draw(*m_Shader);
}

void test::TestModel::OnImGuiRender()
{

}

void test::TestModel::OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset)
{
	m_Camera->OnProcessMouseMovement(xoffset, yoffset);
}

void test::TestModel::OnScroll(GLfloat xoffset, GLfloat yoffset)
{
	m_Camera->OnScroll(xoffset, yoffset);
}

void test::TestModel::OnProcessInput(GLFWwindow* window, GLfloat deltaTime)
{
	m_Camera->OnProcessInput(window, deltaTime);
}
