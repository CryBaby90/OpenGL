#include "TestBloom.h"
#include "Global.h"

#include <stb_image/stb_image.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <sstream>

test::TestBloom::TestBloom()
	:m_BloomShader(nullptr), m_LightShader(nullptr)
{
	m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 5.0f));

	m_WoodTexture = LoadImage("res/textures/wood.png");
	m_ContainerTexture = LoadImage("res/textures/container.jpg");

	// 着色器程序
	m_BloomShader = std::make_unique<Shader>("res/shaders/Bloom/BloomVertex.Vshader", "res/shaders/Bloom/BloomFragment.Fshader");
	m_LightShader = std::make_unique<Shader>("res/shaders/Bloom/BloomVertex.Vshader", "res/shaders/Bloom/LightFragment.Fshader");
	m_BlurShader = std::make_unique<Shader>("res/shaders/Bloom/BlurVertex.Vshader", "res/shaders/Bloom/BlurFragment.Fshader");
	m_FinalShader = std::make_unique<Shader>("res/shaders/Bloom/FinalVertex.Vshader", "res/shaders/Bloom/FinalFragment.Fshader");
	
	glGenFramebuffers(1, &m_HdrFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, m_HdrFBO);
	glGenTextures(2, m_ColorBuffers);
	//两个帧缓冲，一个正常渲染，一个渲染高亮部分
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindTexture(GL_TEXTURE_2D, m_ColorBuffers[i]);
		// 浮点帧缓冲
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);  // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		// attach texture to framebuffer
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + i, GL_TEXTURE_2D, m_ColorBuffers[i], 0);
	}
	//创建渲染缓冲对象
	//如果你不需要从一个缓冲中采样数据，那么对这个缓冲使用渲染缓冲对象会是明智的选择。
	//如果你需要从缓冲中采样颜色或深度值等数据，那么你应该选择纹理附件
	glGenRenderbuffers(1, &m_DepthRBO); //renderBuffer 
	glBindRenderbuffer(GL_RENDERBUFFER, m_DepthRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_DepthRBO);
	//需要显式告知OpenGL我们正在通过glDrawBuffers渲染到多个颜色缓冲
	glDrawBuffers(2, m_Attachments); //颜色附件，分别对应两个帧缓冲
	// finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	//高斯模糊 两个帧缓冲交替
	glGenFramebuffers(2, m_PingpongFBO);
	glGenTextures(2, m_PingpongColorbuffers);
	for (unsigned int i = 0; i < 2; i++)
	{
		glBindFramebuffer(GL_FRAMEBUFFER, m_PingpongFBO[i]);
		glBindTexture(GL_TEXTURE_2D, m_PingpongColorbuffers[i]);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE); // we clamp to the edge as the blur filter would otherwise sample repeated texture values!
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_PingpongColorbuffers[i], 0);
		// also check if framebuffers are complete (no need for depth buffer)
		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
			std::cout << "Framebuffer not complete!" << std::endl;
	}

	// back light
	m_LightPositions.push_back(glm::vec3(0.0f, 0.5f, 1.5f));
	m_LightPositions.push_back(glm::vec3(-4.0f, 0.5f, -3.0f));
	m_LightPositions.push_back(glm::vec3(3.0f, 0.5f, 1.0f));
	m_LightPositions.push_back(glm::vec3(-.8f, 2.4f, -1.0f));

	// colors
	m_LightColors.push_back(glm::vec3(5.0f, 5.0f, 5.0f));
	m_LightColors.push_back(glm::vec3(10.0f, 0.0f, 0.0f));
	m_LightColors.push_back(glm::vec3(0.0f, 0.0f, 15.0f));
	m_LightColors.push_back(glm::vec3(0.0f, 5.0f, 0.0f));

	m_BloomShader->Bind();
	m_BloomShader->SetUniform1i("diffuseTexture", 0);
	m_BloomShader->Unbind();

	m_BlurShader->Bind();
	m_BlurShader->SetUniform1i("image", 0);
	m_BlurShader->Unbind();

	m_FinalShader->Bind();
	m_FinalShader->SetUniform1i("scene", 0);
	m_FinalShader->SetUniform1i("bloomBlur", 0);
	m_FinalShader->Unbind();

	//要开启深度测试 ！！！
	GLCall(glEnable(GL_DEPTH_TEST));
}

test::TestBloom::~TestBloom()
{
	//资源释放
	GLCall(glDeleteVertexArrays(1, &m_QuadVAO));
	GLCall(glDeleteBuffers(1, &m_QuadVBO));
	GLCall(glDeleteVertexArrays(1, &m_CubeVAO));
	GLCall(glDeleteBuffers(1, &m_CubeVBO));
	GLCall(glDeleteFramebuffers(1, &m_HdrFBO));
	GLCall(glDeleteFramebuffers(2, m_PingpongFBO));
}

void test::TestBloom::OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset)
{
	m_Camera->OnProcessMouseMovement(xoffset, yoffset);
}

void test::TestBloom::OnProcessInput(GLFWwindow* window, GLfloat deltaTime)
{
	m_Camera->OnProcessInput(window, deltaTime);

	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && !bloomKeyPressed)
	{
		bloom = !bloom;
		bloomKeyPressed = true;
	}
	if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_RELEASE)
	{
		bloomKeyPressed = false;
	}

	if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS)
	{
		if (exposure > 0.0f)
			exposure -= 0.001f;
		else
			exposure = 0.0f;
	}
	else if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS)
	{
		exposure += 0.001f;
	}
}

int test::TestBloom::LoadImage(char const* filename)
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

int test::TestBloom::LoadImage(char const* filename, bool gammaCorrection)
{
	unsigned int textureID;
	GLCall(glGenTextures(1, &textureID));

	//加载并生成纹理
	GLint width, height, nrChannels;
	//stbi_set_flip_vertically_on_load(true); //图文倒置
	unsigned char* textureData = stbi_load(filename, &width, &height, &nrChannels, 0);
	if (textureData)
	{
		GLenum internalFormat;
		GLenum dataFormat;
		if (nrChannels == 1)
		{
			internalFormat = dataFormat = GL_RED;
		}
		else if (nrChannels == 3)
		{
			internalFormat = gammaCorrection ? GL_SRGB : GL_RGB;
			dataFormat = GL_RGB;
		}
		else if (nrChannels == 4)
		{
			internalFormat = gammaCorrection ? GL_SRGB_ALPHA : GL_RGBA;
			dataFormat = GL_RGBA;
		}

		GLCall(glBindTexture(GL_TEXTURE_2D, textureID));
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, width, height, 0, dataFormat, GL_UNSIGNED_BYTE, textureData));
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

void test::TestBloom::RenderQuad()
{
	if (m_QuadVAO == 0)
	{
		float quadVertices[] = {
			// positions        // texture Coords
			-1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
			-1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			 1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
		};
		// setup plane VAO
		glGenVertexArrays(1, &m_QuadVAO);
		glGenBuffers(1, &m_QuadVBO);
		glBindVertexArray(m_QuadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
	}
	glBindVertexArray(m_QuadVAO);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	glBindVertexArray(0);
}

void test::TestBloom::RenderCube()
{
	if (m_CubeVAO == 0)
	{
		float vertices[] = {
			// back face
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right         
			 1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
			-1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
			-1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
			// front face
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			 1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
			-1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
			-1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
			// left face
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			-1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
			-1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
			// right face
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right         
			 1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
			 1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
			 1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left     
			// bottom face
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			 1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			 1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
			-1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
			-1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
			// top face
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			 1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			 1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right     
			 1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
			-1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
			-1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left        
		};
		glGenVertexArrays(1, &m_CubeVAO);
		glGenBuffers(1, &m_CubeVBO);
		// fill buffer
		glBindBuffer(GL_ARRAY_BUFFER, m_CubeVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
		// link vertex attributes
		glBindVertexArray(m_CubeVAO);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}
	// render Cube
	glBindVertexArray(m_CubeVAO);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	glBindVertexArray(0);
}

void test::TestBloom::OnUpdate(float deltaTime)
{
	
}

void test::TestBloom::OnRender()
{
	GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	//1. 绘制到 浮点帧缓冲 
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_HdrFBO));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	m_BloomShader->Bind();
	m_Proj = m_Camera->GetProjMatrix();
	m_View = m_Camera->GetViewMatrix();
	m_BloomShader->SetUniformsMat4f("projection", m_Proj);
	m_BloomShader->SetUniformsMat4f("view", m_View);
	for (unsigned int i = 0; i < m_LightPositions.size(); i++)
	{
		m_BloomShader->SetUniforms3f("lights[" + std::to_string(i) + "].Position", m_LightPositions[i]);
		m_BloomShader->SetUniforms3f("lights[" + std::to_string(i) + "].Color", m_LightColors[i]);
	}
	m_BloomShader->SetUniforms3f("viewPos", m_Camera->GetPos());
	// 绘制地板
	m_Model = glm::mat4(1.0f);
	m_Model = glm::translate(m_Model, glm::vec3(0.0f, -1.0f, 0.0));
	m_Model = glm::scale(m_Model, glm::vec3(12.5f, 0.5f, 12.5f));
	m_BloomShader->SetUniformsMat4f("model", m_Model);
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_WoodTexture));
	RenderCube();
	//绘制Cube
	GLCall(glBindTexture(GL_TEXTURE_2D, m_ContainerTexture));
	m_Model = glm::mat4(1.0f);
	m_Model = glm::translate(m_Model, glm::vec3(0.0f, 1.5f, 0.0));
	m_Model = glm::scale(m_Model, glm::vec3(0.5f));
	m_BloomShader->SetUniformsMat4f("model", m_Model);
	RenderCube();

	m_Model = glm::mat4(1.0f);
	m_Model = glm::translate(m_Model, glm::vec3(2.0f, 0.0f, 1.0));
	m_Model = glm::scale(m_Model, glm::vec3(0.5f));
	m_BloomShader->SetUniformsMat4f("model", m_Model);
	RenderCube();

	m_Model = glm::mat4(1.0f);
	m_Model = glm::translate(m_Model, glm::vec3(-1.0f, -1.0f, 2.0));
	m_Model = glm::rotate(m_Model, glm::radians(60.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	m_BloomShader->SetUniformsMat4f("model", m_Model);
	RenderCube();

	m_Model = glm::mat4(1.0f);
	m_Model = glm::translate(m_Model, glm::vec3(0.0f, 2.7f, 4.0));
	m_Model = glm::rotate(m_Model, glm::radians(23.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	m_Model = glm::scale(m_Model, glm::vec3(1.25));
	m_BloomShader->SetUniformsMat4f("model", m_Model);
	RenderCube();

	m_Model = glm::mat4(1.0f);
	m_Model = glm::translate(m_Model, glm::vec3(-2.0f, 1.0f, -3.0));
	m_Model = glm::rotate(m_Model, glm::radians(124.0f), glm::normalize(glm::vec3(1.0, 0.0, 1.0)));
	m_BloomShader->SetUniformsMat4f("model", m_Model);
	RenderCube();

	m_Model = glm::mat4(1.0f);
	m_Model = glm::translate(m_Model, glm::vec3(-3.0f, 0.0f, 0.0));
	m_Model = glm::scale(m_Model, glm::vec3(0.5f));
	m_BloomShader->SetUniformsMat4f("model", m_Model);
	RenderCube();
	m_BloomShader->Unbind();

	// 绘制高光Cubes
	m_LightShader->Bind();
	m_LightShader->SetUniformsMat4f("projection", m_Proj);
	m_LightShader->SetUniformsMat4f("view", m_View);
	for (unsigned int i = 0; i < m_LightPositions.size(); i++)
	{
		m_Model = glm::mat4(1.0f);
		m_Model = glm::translate(m_Model, glm::vec3(m_LightPositions[i]));
		m_Model = glm::scale(m_Model, glm::vec3(0.25f));
		m_LightShader->SetUniformsMat4f("model", m_Model);
		m_LightShader->SetUniforms3f("lightColor", m_LightColors[i]);
		RenderCube();
	}
	m_LightShader->Unbind();
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	//2. 两个颜色缓冲做高斯模糊
	bool horizontal = true, first_iteration = true;
	unsigned int amount = 10;
	m_BlurShader->Bind();
	for (unsigned int i = 0; i < amount; i++)
	{
		GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_PingpongFBO[horizontal]));
		m_BlurShader->SetUniform1i("horizontal", horizontal);
		// bind texture of other framebuffer (or scene if first iteration)
		GLCall(glBindTexture(GL_TEXTURE_2D, first_iteration ? m_ColorBuffers[1] : m_PingpongColorbuffers[!horizontal]));  
		RenderQuad();
		horizontal = !horizontal;
		if (first_iteration)
			first_iteration = false;
	}
	m_BlurShader->Unbind();
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	//合并绘制帧缓冲到 2D quad
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	m_FinalShader->Bind();
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_ColorBuffers[0]));
	GLCall(glActiveTexture(GL_TEXTURE1));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_PingpongColorbuffers[!horizontal]));
	m_FinalShader->SetUniform1i("bloom", bloom);
	m_FinalShader->SetUniform1f("exposure", exposure);
	RenderQuad();
}

void test::TestBloom::OnImGuiRender()
{
	//ImGui::SliderFloat3("ViewPos", &m_ViewPos.x, -6.0f, 6.0f);
}
