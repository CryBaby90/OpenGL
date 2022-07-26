#include "TestMultipleLights.h"
#include "Global.h"

#include <stb_image/stb_image.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

test::TestMultipleLights::TestMultipleLights()
	:m_LightingShader(nullptr), m_CubeShader(nullptr), m_Camera(nullptr),
	m_Model(glm::mat4(1.0f)), m_View(glm::mat4(1.0f)), m_Proj(glm::mat4(1.0f)), m_Normal(glm::mat3(1.0f)),
	m_LightPos(glm::vec3(0.0f, 0.0f, 0.0f))
{
	//在上下文之后
	GLfloat vertices[] = {
	// positions          // normals           // texture coords
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f,  1.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  1.0f,
	-0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f,  0.0f,

	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  1.0f,  1.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f,  0.0f,

	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	-0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	-0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f,  0.0f,

	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
	 0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	 0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
	-0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f,  1.0f,

	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
	 0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  1.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	 0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f,
	-0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
	-0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f
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
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(0));
	//法线pointer
	GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))));
	GLCall(glEnableVertexAttribArray(1));
	//贴图pointer
	GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))));
	GLCall(glEnableVertexAttribArray(2));

	//创建并绑定lightVAO
	GLCall(glGenVertexArrays(1, &m_lightVAO));
	GLCall(glBindVertexArray(m_lightVAO));

	//只需要绑定VBO不用再次设置VBO的数据，因为cube的VBO数据中已经包含了正确的立方体顶点数据
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(0));

	m_CubeShader = std::make_unique<Shader>("res/shaders/MultipleLights/Vertex.Vshader", "res/shaders/MultipleLights/CubeFragement.Fshader");
	m_LightingShader = std::make_unique<Shader>("res/shaders/MultipleLights/Vertex.Vshader", "res/shaders/MultipleLights/LightingFragement.Fshader");

	//开启z-buffer 见OpenGL 坐标系统
	GLCall(glEnable(GL_DEPTH_TEST));

	m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 6.0f));

	GLCall(glBindVertexArray(0));

	m_LightPos = glm::vec3(1.2f, 1.0f, 2.0f);

	m_DiffuseMapID = loadTexture("res/textures/DiffuseMap.png");
	m_SpecularMapID = loadTexture("res/textures/SpecularMap.png");

	m_CubePositions = new glm::vec3[]{
	glm::vec3(0.0f,  0.0f,  0.0f),
	glm::vec3(2.0f,  5.0f, -15.0f),
	glm::vec3(-1.5f, -2.2f, -2.5f),
	glm::vec3(-3.8f, -2.0f, -12.3f),
	glm::vec3(2.4f, -0.4f, -3.5f),
	glm::vec3(-1.7f,  3.0f, -7.5f),
	glm::vec3(1.3f, -2.0f, -2.5f),
	glm::vec3(1.5f,  2.0f, -2.5f),
	glm::vec3(1.5f,  0.2f, -1.5f),
	glm::vec3(-1.3f,  1.0f, -1.5f)
	};
}

test::TestMultipleLights::~TestMultipleLights()
{
	//资源释放
	delete m_CubePositions;
	GLCall(glDeleteVertexArrays(1, &m_CubeVAO));
	GLCall(glDeleteVertexArrays(1, &m_lightVAO));
	GLCall(glDeleteBuffers(1, &m_VBO));
}

void test::TestMultipleLights::OnUpdate(float deltaTime)
{
	m_View = m_Camera->GetViewMatrix();
	m_Proj = m_Camera->GetProjMatrix();
}

void test::TestMultipleLights::OnRender()
{
	glm::vec3 pointLightPositions[] = {
	glm::vec3(0.7f,  0.2f,  2.0f),
	glm::vec3(2.3f, -3.3f, -4.0f),
	glm::vec3(-4.0f,  2.0f, -12.0f),
	glm::vec3(0.0f,  0.0f, -3.0f)
	};

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
	m_CubeShader->SetUniforms3f("viewPos", m_Camera->GetPos());

	//m_CubeShader->SetUniforms3f("material.ambient", glm::vec3(0.0f, 0.1f, 0.06f));
	m_CubeShader->SetUniform1i("material.diffuse", 0);//使用0号纹理插槽
	m_CubeShader->SetUniform1i("material.specular", 1);//使用1号纹理插槽
	m_CubeShader->SetUniform1f("material.shininess", 64.0f);

	//平行光
	m_CubeShader->SetUniforms3f("dirLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
	m_CubeShader->SetUniforms3f("dirLight.ambient", glm::vec3(0.05f, 0.05f, 0.05f));
	m_CubeShader->SetUniforms3f("dirLight.diffuse", glm::vec3(0.4f, 0.4f, 0.4f));
	m_CubeShader->SetUniforms3f("dirLight.specular", glm::vec3(0.5f, 0.5f, 0.5f));

	//点光源1
	m_CubeShader->SetUniform1f("pointLights[0].constant", 1.0f);
	m_CubeShader->SetUniform1f("pointLights[0].linear", 0.09f);
	m_CubeShader->SetUniform1f("pointLights[0].quadratic", 0.032f);
	m_CubeShader->SetUniforms3f("pointLights[0].position", pointLightPositions[0]);
	m_CubeShader->SetUniforms3f("pointLights[0].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	m_CubeShader->SetUniforms3f("pointLights[0].diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	m_CubeShader->SetUniforms3f("pointLights[0].specular", glm::vec3(1.0f, 1.0f, 1.0f));

	//点光源2
	m_CubeShader->SetUniform1f("pointLights[1].constant", 1.0f);
	m_CubeShader->SetUniform1f("pointLights[1].linear", 0.09f);
	m_CubeShader->SetUniform1f("pointLights[1].quadratic", 0.032f);
	m_CubeShader->SetUniforms3f("pointLights[1].position", pointLightPositions[1]);
	m_CubeShader->SetUniforms3f("pointLights[1].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	m_CubeShader->SetUniforms3f("pointLights[1].diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	m_CubeShader->SetUniforms3f("pointLights[1].specular", glm::vec3(1.0f, 1.0f, 1.0f));

	//点光源3
	m_CubeShader->SetUniform1f("pointLights[2].constant", 1.0f);
	m_CubeShader->SetUniform1f("pointLights[2].linear", 0.09f);
	m_CubeShader->SetUniform1f("pointLights[2].quadratic", 0.032f);
	m_CubeShader->SetUniforms3f("pointLights[2].position", pointLightPositions[2]);
	m_CubeShader->SetUniforms3f("pointLights[2].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	m_CubeShader->SetUniforms3f("pointLights[2].diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	m_CubeShader->SetUniforms3f("pointLights[2].specular", glm::vec3(1.0f, 1.0f, 1.0f));

	//点光源4
	m_CubeShader->SetUniform1f("pointLights[3].constant", 1.0f);
	m_CubeShader->SetUniform1f("pointLights[3].linear", 0.09f);
	m_CubeShader->SetUniform1f("pointLights[3].quadratic", 0.032f);
	m_CubeShader->SetUniforms3f("pointLights[3].position", pointLightPositions[3]);
	m_CubeShader->SetUniforms3f("pointLights[3].ambient", glm::vec3(0.2f, 0.2f, 0.2f));
	m_CubeShader->SetUniforms3f("pointLights[3].diffuse", glm::vec3(0.5f, 0.5f, 0.5f));
	m_CubeShader->SetUniforms3f("pointLights[3].specular", glm::vec3(1.0f, 1.0f, 1.0f));

	//聚光灯 切光角
	m_CubeShader->SetUniform1f("spotLight.constant", 1.0f);
	m_CubeShader->SetUniform1f("spotLight.linear", 0.09f);
	m_CubeShader->SetUniform1f("spotLight.quadratic", 0.032f);
	m_CubeShader->SetUniform1f("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
	m_CubeShader->SetUniform1f("spotLight.outerCutOff", glm::cos(glm::radians(17.5f)));
	m_CubeShader->SetUniforms3f("spotLight.position", m_LightPos);
	m_CubeShader->SetUniforms3f("spotLight.direction", glm::vec3(-0.2f, -1.0f, -0.3f));
	m_CubeShader->SetUniforms3f("spotLight.ambient", glm::vec3(0.0f, 0.0f, 0.0f));
	m_CubeShader->SetUniforms3f("spotLight.diffuse", glm::vec3(1.0f, 1.0f, 1.0f));
	m_CubeShader->SetUniforms3f("spotLight.specular", glm::vec3(1.0f, 1.0f, 1.0f));

	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_DiffuseMapID));
	GLCall(glActiveTexture(GL_TEXTURE1));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_SpecularMapID));

	GLCall(glBindVertexArray(m_CubeVAO));
	//GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
	//GLCall(glDrawArrays(GL_TRIANGLES, 0, 36)); //没有设置index pos  暂时用glDrawArrays
	for (unsigned int i = 0; i < 10; i++)
	{
		glm::mat4 model(glm::mat4(1.0f));
		model = glm::translate(model, m_CubePositions[i]);
		float angle = 20.0f * i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		m_CubeShader->SetUniformsMat4f("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}


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
	for (unsigned int i = 0; i < 4; i++)
	{
		glm::mat4 model(glm::mat4(1.0f));
		model = glm::translate(model, pointLightPositions[i]);
		model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
		m_LightingShader->SetUniformsMat4f("model", model);
		GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
	}
	

	GLCall(glBindVertexArray(0));
}

void test::TestMultipleLights::OnImGuiRender()
{

}

unsigned int test::TestMultipleLights::loadTexture(char const* path)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);

	int width, height, nrComponents;
	unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
	if (data)
	{
		GLenum format = GL_RGBA;
		if (nrComponents == 1)
			format = GL_RED;
		else if (nrComponents == 3)
			format = GL_RGB;
		else if (nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureID);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		stbi_image_free(data);
	}
	else
	{
		std::cout << "Texture failed to load at path: " << path << std::endl;
		stbi_image_free(data);
	}

	return textureID;
}

void test::TestMultipleLights::OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset)
{
	//m_Camera->OnProcessMouseMovement(xoffset, yoffset);
}

void test::TestMultipleLights::OnScroll(GLfloat xoffset, GLfloat yoffset)
{
	m_Camera->OnScroll(xoffset, yoffset);
}

void test::TestMultipleLights::OnProcessInput(GLFWwindow* window, GLfloat deltaTime)
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
