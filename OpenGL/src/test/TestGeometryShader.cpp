#include "TestGeometryShader.h"

#include <stb_image/stb_image.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>

test::TestGeometryShader::TestGeometryShader()
	:m_Shader(nullptr), m_NormalShader(nullptr), m_Camera(nullptr), m_Model(nullptr)
{
	//在上下文之后
	//float points[] = {
	//-0.5f,  0.5f, // 左上
	// 0.5f,  0.5f, // 右上
	// 0.5f, -0.5f, // 右下
	//-0.5f, -0.5f  // 左下
	//};

	float points[] = {
		-0.5f,  0.5f, 1.0f, 0.0f, 0.0f, // 左上
		 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, // 右上
		 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, // 右下
		-0.5f, -0.5f, 1.0f, 1.0f, 0.0f  // 左下
	};

	//  VAO
	GLCall(glGenVertexArrays(1, &m_CubeVAO));
	GLCall(glGenBuffers(1, &m_CubeVBO));
	GLCall(glBindVertexArray(m_CubeVAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_CubeVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(points), points, GL_STATIC_DRAW));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(2 * sizeof(float)));
	GLCall(glBindVertexArray(0));

	//6  着色器程序
	m_Shader = std::make_unique<Shader>("res/shaders/GeometryShader/Vertex.Vshader", "res/shaders/GeometryShader/Fragment.Fshader");
	m_NormalShader = std::make_unique<Shader>("res/shaders/GeometryShader/NormalShaderVertex.Vshader", "res/shaders/GeometryShader/NormalShaderFragment.Fshader", "res/shaders/GeometryShader/Geometry.Gshader");
	m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));

	m_Model = std::make_unique<Model>("res/obj/ReflectionObj/nanosuit.obj");

	GLCall(glEnable(GL_DEPTH_TEST));
}

test::TestGeometryShader::~TestGeometryShader()
{
	//资源释放
	GLCall(glDeleteVertexArrays(1, &m_CubeVAO));
	GLCall(glDeleteBuffers(1, &m_CubeVBO));
}

void test::TestGeometryShader::OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset)
{
	m_Camera->OnProcessMouseMovement(xoffset, yoffset);
}

void test::TestGeometryShader::OnProcessInput(GLFWwindow* window, GLfloat deltaTime)
{
	m_Camera->OnProcessInput(window, deltaTime);
}

int test::TestGeometryShader::LoadImage(char const* filename)
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

int test::TestGeometryShader::LoadCubemap(std::vector<std::string> faces)
{
	unsigned int textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

	int width, height, nrChannels;
	for (unsigned int i = 0; i < faces.size(); i++)
	{
		unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
				0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data
			);
			stbi_image_free(data);
		}
		else
		{
			std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
			stbi_image_free(data);
		}
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

	return textureID;
}

void test::TestGeometryShader::OnUpdate(float deltaTime)
{
	
}

void test::TestGeometryShader::OnRender()
{
	GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	m_Shader->Bind();
	glm::mat4 proj = m_Camera->GetProjMatrix();
	glm::mat4 view = m_Camera->GetViewMatrix();;
	glm::mat4 model = glm::mat4(1.0f);
	model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
	m_Shader->SetUniformsMat4f("proj", proj);
	m_Shader->SetUniformsMat4f("view", view);
	m_Shader->SetUniformsMat4f("model", model);
	//m_Shader->SetUniform1f("time", static_cast<float>(glfwGetTime()));
	//GLCall(glBindVertexArray(m_CubeVAO));
	//glDrawArrays(GL_POINTS, 0, 4);
	m_Model->Draw(*m_Shader);

	m_NormalShader->Bind();
	m_NormalShader->SetUniformsMat4f("proj", proj);
	m_NormalShader->SetUniformsMat4f("view", view);
	m_NormalShader->SetUniformsMat4f("model", model);
	m_Model->Draw(*m_NormalShader);
	GLCall(glBindVertexArray(0));
}

void test::TestGeometryShader::OnImGuiRender()
{
	//ImGui::SliderFloat3("ViewPos", &m_ViewPos.x, -6.0f, 6.0f);
}
