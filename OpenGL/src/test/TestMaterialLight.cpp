#include "TestMaterialLight.h"
#include "Global.h"

#include <stb_image/stb_image.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

test::TestMaterialLight::TestMaterialLight()
	:m_LightingShader(nullptr), m_CubeShader(nullptr), m_Camera(nullptr),
	m_Model(glm::mat4(1.0f)), m_View(glm::mat4(1.0f)), m_Proj(glm::mat4(1.0f)), m_Normal(glm::mat3(1.0f)),
	m_LightPos(glm::vec3(0.0f, 0.0f, 0.0f))
{
	//在上下文之后
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


	//  创建CubeVAO, VBO
	GLCall(glGenVertexArrays(1, &m_CubeVAO));
	GLCall(glGenBuffers(1, &m_VBO));

	//绑定CubeVAO缓冲对象
	GLCall(glBindVertexArray(m_CubeVAO));

	//绑定VBO（顶点数组）到缓冲中供OpenGL使用
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

	//设置顶点属性指针
	//顶点pointer
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(0));
	//法线pointer
	GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(1));

	//创建并绑定lightVAO
	GLCall(glGenVertexArrays(1, &m_lightVAO));
	GLCall(glBindVertexArray(m_lightVAO));

	//只需要绑定VBO不用再次设置VBO的数据，因为cube的VBO数据中已经包含了正确的立方体顶点数据
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(0));

	m_CubeShader = std::make_unique<Shader>("res/shaders/MaterialLight/Vertex.Vshader", "res/shaders/MaterialLight/CubeFragement.Fshader");
	m_LightingShader = std::make_unique<Shader>("res/shaders/MaterialLight/Vertex.Vshader", "res/shaders/MaterialLight/LightingFragement.Fshader");

	//开启z-buffer 见OpenGL 坐标系统
	GLCall(glEnable(GL_DEPTH_TEST));

	m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));

	GLCall(glBindVertexArray(0));

	m_LightPos = glm::vec3(1.2f, 1.0f, 2.0f);
}

test::TestMaterialLight::~TestMaterialLight()
{
	//资源释放
	GLCall(glDeleteVertexArrays(1, &m_CubeVAO));
	GLCall(glDeleteVertexArrays(1, &m_lightVAO));
	GLCall(glDeleteBuffers(1, &m_VBO));
}

void test::TestMaterialLight::OnUpdate(float deltaTime)
{
	m_View = m_Camera->GetViewMatrix();
	m_Proj = m_Camera->GetProjMatrix();
	//float timeValue = static_cast<GLfloat>(glfwGetTime());
	//m_LightPos += glm::vec3(cos(timeValue), sin(timeValue), 0.0f) * 2.5f * deltaTime;
}

void test::TestMaterialLight::OnRender()
{
	//每次渲染迭代前清除之前的深度缓存
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	//先use shader
	m_CubeShader->Bind();
	//给shader里的变量指定插槽
	m_Model = glm::mat4(1.0f);
	m_Model = glm::rotate(m_Model, 1.0f, glm::vec3(0.5f, 0.5f, 0.0f));
	//m_Normal = glm::transpose(glm::inverse(m_Model)); //世界空间
	m_Normal = glm::transpose(glm::inverse(m_View * m_Model)); //观察空间
	m_CubeShader->SetUniformsMat4f("model", m_Model);
	m_CubeShader->SetUniformsMat4f("view", m_View);
	m_CubeShader->SetUniformsMat4f("proj", m_Proj);
	m_CubeShader->SetUniformsMat3f("normal", m_Normal);
	m_CubeShader->SetUniforms3f("lightPos", m_LightPos);
	m_CubeShader->SetUniforms3f("viewPos", m_Camera->GetPos());
	m_CubeShader->SetUniforms3f("material.ambient", glm::vec3(0.0f, 0.1f, 0.06f));
	m_CubeShader->SetUniforms3f("material.diffuse", glm::vec3(0.0f, 0.50980392f, 0.50980392f));
	m_CubeShader->SetUniforms3f("material.specular", glm::vec3(0.50196078f, 0.50196078f, 0.50196078f));
	m_CubeShader->SetUniform1f("material.shininess", 32.0f);
	//glm::vec3 lightColor;
	//lightColor.x = sin(glfwGetTime() * 2.0f);
	//lightColor.y = sin(glfwGetTime() * 0.7f);
	//lightColor.z = sin(glfwGetTime() * 1.3f);
	//glm::vec3 diffuseColor = lightColor * glm::vec3(0.5f); // 降低影响
	//glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // 很低的影响
	//m_CubeShader->SetUniforms3f("light.ambient", ambientColor);
	//m_CubeShader->SetUniforms3f("light.diffuse", diffuseColor);
	m_CubeShader->SetUniforms3f("light.ambient", glm::vec3(1.0f, 1.0f, 1.0f));
	m_CubeShader->SetUniforms3f("light.diffuse", glm::vec3(1.0f, 1.0f, 1.0f)); // 将光照调暗了一些以搭配场景
	m_CubeShader->SetUniforms3f("light.specular", glm::vec3(1.0f, 1.0f, 1.0f));
	GLCall(glBindVertexArray(m_CubeVAO));
	//GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36)); //没有设置index pos  暂时用glDrawArrays

	//先use shader
	m_LightingShader->Bind();
	//给shader里的变量指定插槽
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

void test::TestMaterialLight::OnImGuiRender()
{

}

void test::TestMaterialLight::OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset)
{
	//m_Camera->OnProcessMouseMovement(xoffset, yoffset);
}

void test::TestMaterialLight::OnScroll(GLfloat xoffset, GLfloat yoffset)
{
	m_Camera->OnScroll(xoffset, yoffset);
}

void test::TestMaterialLight::OnProcessInput(GLFWwindow* window, GLfloat deltaTime)
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
