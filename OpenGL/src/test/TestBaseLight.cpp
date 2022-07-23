#include "TestBaseLight.h"
#include "Global.h"

#include <stb_image/stb_image.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

test::TestBaseLight::TestBaseLight()
	:m_LightingShader(nullptr), m_CubeShader(nullptr), m_Camera(nullptr),
	m_Model(glm::mat4(1.0f)), m_View(glm::mat4(1.0f)), m_Proj(glm::mat4(1.0f)), m_Normal(glm::mat3(1.0f)),
	m_LightPos(glm::vec3(0.0f, 0.0f, 0.0f))
{
	//��������֮��
	GLfloat vertices[] = {
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
	};


	//  ����CubeVAO, VBO
	GLCall(glGenVertexArrays(1, &m_CubeVAO));
	GLCall(glGenBuffers(1, &m_VBO));

	//��CubeVAO�������
	GLCall(glBindVertexArray(m_CubeVAO));

	//��VBO���������飩�������й�OpenGLʹ��
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
	
	//���ö�������ָ��
	//����pointer
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(0));
	//����pointer
	GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(1));

	//��������lightVAO
	GLCall(glGenVertexArrays(1, &m_lightVAO));
	GLCall(glBindVertexArray(m_lightVAO));

	//ֻ��Ҫ��VBO�����ٴ�����VBO�����ݣ���Ϊcube��VBO�������Ѿ���������ȷ�������嶥������
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(0));
	
	 m_CubeShader = std::make_unique<Shader>("res/shaders/BaseLight/Vertex.Vshader", "res/shaders/BaseLight/LightingFragement.Fshader");
	 m_LightingShader = std::make_unique<Shader>("res/shaders/BaseLight/Vertex.Vshader", "res/shaders/BaseLight/CubeFragement.Fshader");
	
	//����z-buffer ��OpenGL ����ϵͳ
	GLCall(glEnable(GL_DEPTH_TEST));

	m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 6.0f));

	GLCall(glBindVertexArray(0));

	m_LightPos = glm::vec3(0.0f, 1.0f, 1.0f);
}

test::TestBaseLight::~TestBaseLight()
{
	//��Դ�ͷ�
	GLCall(glDeleteVertexArrays(1, &m_CubeVAO));
	GLCall(glDeleteVertexArrays(1, &m_lightVAO));
	GLCall(glDeleteBuffers(1, &m_VBO));
}

void test::TestBaseLight::OnUpdate(float deltaTime)
{
	m_View = m_Camera->GetViewMatrix();
	m_Proj = m_Camera->GetProjMatrix();
}

void test::TestBaseLight::OnRender()
{
	//ÿ����Ⱦ����ǰ���֮ǰ����Ȼ���
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	//��use shader
	m_CubeShader->Bind();
	//��shader��ı���ָ�����
	m_Model = glm::mat4(1.0f);
	m_Model = glm::rotate(m_Model, glm::radians(static_cast<float>(glfwGetTime()) * 30), glm::vec3(0.5f, 0.5f, 0.0f));
	m_Normal = glm::transpose(glm::inverse(m_Model));
	m_CubeShader->SetUniformsMat4f("model", m_Model);
	m_CubeShader->SetUniformsMat4f("view", m_View);
	m_CubeShader->SetUniformsMat4f("proj", m_Proj);
	m_CubeShader->SetUniformsMat3f("normal", m_Normal);
	m_CubeShader->SetUniforms3f("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
	m_CubeShader->SetUniforms3f("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	m_CubeShader->SetUniforms3f("lightPos", m_LightPos);
	m_CubeShader->SetUniforms3f("viewPos", m_Camera->GetPos());
	GLCall(glBindVertexArray(m_CubeVAO));
	//GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36)); //û������index pos  ��ʱ��glDrawArrays

	//��use shader
	m_LightingShader->Bind();
	//��shader��ı���ָ�����
	m_Model = glm::mat4(1.0f);
	m_Model = glm::translate(m_Model, m_LightPos);
	m_Model = glm::scale(m_Model, glm::vec3(0.1f));
	m_LightingShader->SetUniformsMat4f("model", m_Model);
	m_LightingShader->SetUniformsMat4f("view", m_View);
	m_LightingShader->SetUniformsMat4f("proj", m_Proj);
	GLCall(glBindVertexArray(m_lightVAO));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));

	GLCall(glBindVertexArray(0));
}

void test::TestBaseLight::OnImGuiRender()
{
	
}

void test::TestBaseLight::OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset)
{
	//m_Camera->OnProcessMouseMovement(xoffset, yoffset);
}

void test::TestBaseLight::OnScroll(GLfloat xoffset, GLfloat yoffset)
{
	m_Camera->OnScroll(xoffset, yoffset);
}

void test::TestBaseLight::OnProcessInput(GLFWwindow* window, GLfloat deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		m_LightPos += glm::vec3(0.0f, 1.0f, 0.0f) * 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		m_LightPos -= glm::vec3(0.0f, 1.0f, 0.0f) * 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		m_LightPos -= glm::vec3(1.0f, 0.0f, 0.0f) * 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		m_LightPos += glm::vec3(1.0f, 0.0f, 0.0f) * 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
		m_LightPos -= glm::vec3(0.0f, 0.0f, 1.0f) * 2.5f * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
		m_LightPos += glm::vec3(0.0f, 0.0f, 1.0f) * 2.5f * deltaTime;
}
