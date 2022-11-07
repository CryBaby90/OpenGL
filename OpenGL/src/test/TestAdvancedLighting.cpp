#include "TestAdvancedLighting.h"
#include "Global.h"

#include <stb_image/stb_image.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <sstream>

bool blinn = false;
bool blinnKeyPressed = false;

test::TestAdvancedLighting::TestAdvancedLighting()
	:m_Shader(nullptr), m_Camera(nullptr)
{
	//在上下文之后
	float planeVertices[] = {
		// positions            // normals         // texcoords
		 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
		-10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,   0.0f,  0.0f,
		-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,

		 10.0f, -0.5f,  10.0f,  0.0f, 1.0f, 0.0f,  10.0f,  0.0f,
		-10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,   0.0f, 10.0f,
		 10.0f, -0.5f, -10.0f,  0.0f, 1.0f, 0.0f,  10.0f, 10.0f
	};

	//  VAO
	GLCall(glGenVertexArrays(1, &m_CubeVAO));
	GLCall(glGenBuffers(1, &m_CubeVBO));
	GLCall(glBindVertexArray(m_CubeVAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_CubeVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float))));
	GLCall(glEnableVertexAttribArray(2));
	GLCall(glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))));
	GLCall(glBindVertexArray(0));

	m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));
	m_FloorTexture = LoadImage("res/textures/wood.png");

	//6  着色器程序
	m_Shader = std::make_unique<Shader>("res/shaders/AdvancedLighting/Vertex.Vshader", "res/shaders/AdvancedLighting/Fragment.Fshader");
	m_Shader->Bind();
	m_Shader->SetUniform1i("floorTexture", 0);
	m_Shader->Unbind();
}

test::TestAdvancedLighting::~TestAdvancedLighting()
{
	//资源释放
	GLCall(glDeleteVertexArrays(1, &m_CubeVAO));
	GLCall(glDeleteBuffers(1, &m_CubeVBO));
}

void test::TestAdvancedLighting::OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset)
{
	m_Camera->OnProcessMouseMovement(xoffset, yoffset);
}

void test::TestAdvancedLighting::OnProcessInput(GLFWwindow* window, GLfloat deltaTime)
{
	m_Camera->OnProcessInput(window, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS && !blinnKeyPressed)
	{
		blinn = !blinn;
		blinnKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_B) == GLFW_RELEASE)
	{
		blinnKeyPressed = false;
	}
}

int test::TestAdvancedLighting::LoadImage(char const* filename)
{
	unsigned int textureID;
	GLCall(glGenTextures(1, &textureID));
	
	//加载并生成纹理
	GLint width, height, nrChannels;
	//stbi_set_flip_vertically_on_load(true); //图文倒置
	unsigned char* textureData = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (textureData)
	{
		GLenum format;
		if (nrChannels == 1)
			format = GL_RED;
		else if (nrChannels == 3)
			format = GL_RGB;
		else if (nrChannels == 4)
			format = GL_RGBA;

		GLCall(glBindTexture(GL_TEXTURE_2D, textureID));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, textureData));
		GLCall(glGenerateMipmap(GL_TEXTURE_2D));

		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT)); 
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	}
	else
	{
		std::cout << "Failed to load texture1" << std::endl;
	}
	stbi_image_free(textureData);

	return textureID;
}

void test::TestAdvancedLighting::OnUpdate(float deltaTime)
{
	
}

void test::TestAdvancedLighting::OnRender()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::vec3 lightPos(0.0f, 0.0f, 0.0f);

	m_Shader->Bind();
	m_View = m_Camera->GetViewMatrix();
	m_Proj = m_Camera->GetProjMatrix();
	
	m_Shader->SetUniforms3f("viewPos", m_Camera->GetPos());
	m_Shader->SetUniforms3f("lightPos", lightPos);
	m_Shader->SetUniform1i("blinn", blinn);

	m_Shader->SetUniformsMat4f("view", m_View);
	m_Shader->SetUniformsMat4f("projection", m_Proj);
	glBindVertexArray(m_CubeVAO);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_FloorTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	std::cout << (blinn ? "Blinn-Phong" : "Phong") << std::endl;
}

void test::TestAdvancedLighting::OnImGuiRender()
{
	//ImGui::SliderFloat3("ViewPos", &m_ViewPos.x, -6.0f, 6.0f);
}
