#include "TestLightColor.h"
#include "Global.h"

#include <stb_image/stb_image.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

test::TestLightColor::TestLightColor()
	:m_LightingShader(nullptr), m_CubeShader(nullptr), m_Camera(nullptr),
	m_Model(glm::mat4(1.0f)), m_View(glm::mat4(1.0f)), m_Proj(glm::mat4(1.0f))
{
	//在上下文之后
	GLfloat vertices[] = {
		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f, -0.5f,
		 0.5f, -0.5f,  0.5f,
		 0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f, -0.5f,
		 0.5f,  0.5f,  0.5f,
		 0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
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
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(0));

	//创建并绑定lightVAO
	GLCall(glGenVertexArrays(1, &m_lightVAO));
	GLCall(glBindVertexArray(m_lightVAO));
	//只需要绑定VBO不用再次设置VBO的数据，因为cube的VBO数据中已经包含了正确的立方体顶点数据
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(0));
	
	 m_CubeShader = std::make_unique<Shader>("res/shaders/LightColor/Vertex.Vshader", "res/shaders/LightColor/LightingFragement.Fshader");
	 m_LightingShader = std::make_unique<Shader>("res/shaders/LightColor/Vertex.Vshader", "res/shaders/LightColor/CubeFragement.Fshader");
	
	//开启z-buffer 见OpenGL 坐标系统
	GLCall(glEnable(GL_DEPTH_TEST));

	m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 6.0f));

	GLCall(glBindVertexArray(0));
}

test::TestLightColor::~TestLightColor()
{
	//资源释放
	GLCall(glDeleteVertexArrays(1, &m_CubeVAO));
	GLCall(glDeleteVertexArrays(1, &m_lightVAO));
	GLCall(glDeleteBuffers(1, &m_VBO));
}

void test::TestLightColor::OnUpdate(float deltaTime)
{
	m_View = m_Camera->GetViewMatrix();
	m_Proj = m_Camera->GetProjMatrix();
}

void test::TestLightColor::OnRender()
{
	//每次渲染迭代前清除之前的深度缓存
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	//先use shader
	m_CubeShader->Bind();
	//给shader里的变量指定插槽
	m_Model = glm::mat4(1.0f);
	m_Model = glm::rotate(m_Model, glm::radians(static_cast<float>(glfwGetTime()) * 30), glm::vec3(1.0f, 0.3f, 0.5f));
	m_CubeShader->SetUniformsMat4f("model", m_Model);
	m_CubeShader->SetUniformsMat4f("view", m_View);
	m_CubeShader->SetUniformsMat4f("proj", m_Proj);
	m_CubeShader->SetUniforms3f("objectColor", glm::vec3(1.0f, 0.5f, 0.31f));
	m_CubeShader->SetUniforms3f("lightColor", glm::vec3(1.0f, 1.0f, 1.0f));
	GLCall(glBindVertexArray(m_CubeVAO));
	//GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36)); //没有设置index pos  暂时用glDrawArrays

	//先use shader
	m_LightingShader->Bind();
	//给shader里的变量指定插槽
	m_Model = glm::mat4(1.0f);
	m_Model = glm::translate(m_Model, glm::vec3(1.2f, 1.0f, 2.0f));
	m_Model = glm::scale(m_Model, glm::vec3(0.2f));
	m_LightingShader->SetUniformsMat4f("model", m_Model);
	m_LightingShader->SetUniformsMat4f("view", m_View);
	m_LightingShader->SetUniformsMat4f("proj", m_Proj);
	GLCall(glBindVertexArray(m_lightVAO));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));

	GLCall(glBindVertexArray(0));
}

void test::TestLightColor::OnImGuiRender()
{
	
}
