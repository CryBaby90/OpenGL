#include "TestDeferredShading.h"
#include "Global.h"

#include <stb_image/stb_image.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <sstream>

test::TestDeferredShading::TestDeferredShading()
	:m_Shader(nullptr), m_BoxShader(nullptr), m_GBufferShader(nullptr), 
	m_Camera(nullptr), m_RealModel(nullptr)
{
	/*
	* 延迟渲染确实带来一些缺点：大内存开销，没有MSAA和混合(仍需要正向渲染的配合)。
	* 当你有一个很小的场景并且没有很多的光源时候，
	* 延迟渲染并不一定会更快一点，甚至有些时候由于开销超过了它的优点还会更慢。
	*/

	m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 1.0f, 7.0f));
	m_RealModel = std::make_unique<Model>("res/obj/nanosuit.obj");

	// 着色器程序
	m_GBufferShader = std::make_unique<Shader>("res/shaders/DeferredShading/GBufferVertex.Vshader", "res/shaders/DeferredShading/GBufferFragment.Fshader");
	m_Shader = std::make_unique<Shader>("res/shaders/DeferredShading/Vertex.Vshader", "res/shaders/DeferredShading/Fragment.Fshader");
	m_BoxShader = std::make_unique<Shader>("res/shaders/DeferredShading/BoxVertex.Vshader", "res/shaders/DeferredShading/BoxFragment.Fshader");

	// back light
	m_ObjectPositions.push_back(glm::vec3(-3.0, -0.5, -3.0));
	m_ObjectPositions.push_back(glm::vec3(0.0, -0.5, -3.0));
	m_ObjectPositions.push_back(glm::vec3(3.0, -0.5, -3.0));
	m_ObjectPositions.push_back(glm::vec3(-3.0, -0.5, 0.0));
	m_ObjectPositions.push_back(glm::vec3(0.0, -0.5, 0.0));
	m_ObjectPositions.push_back(glm::vec3(3.0, -0.5, 0.0));
	m_ObjectPositions.push_back(glm::vec3(-3.0, -0.5, 3.0));
	m_ObjectPositions.push_back(glm::vec3(0.0, -0.5, 3.0));
	m_ObjectPositions.push_back(glm::vec3(3.0, -0.5, 3.0));

	// colors
	const GLuint NR_LIGHTS = 32;
	srand(13);
	for (GLuint i = 0; i < NR_LIGHTS; i++)
	{
		// Calculate slightly random offsets
		GLfloat xPos = ((rand() % 100) / 100.0f) * 6.0f - 3.0f;
		GLfloat yPos = ((rand() % 100) / 100.0f) * 6.0f - 4.0f;
		GLfloat zPos = ((rand() % 100) / 100.0f) * 6.0f - 3.0f;
		m_LightPositions.push_back(glm::vec3(xPos, yPos, zPos));
		// Also calculate random color
		GLfloat rColor = ((rand() % 100) / 200.0f) + 0.5f; // Between 0.5 and 1.0
		GLfloat gColor = ((rand() % 100) / 200.0f) + 0.5f; // Between 0.5 and 1.0
		GLfloat bColor = ((rand() % 100) / 200.0f) + 0.5f; // Between 0.5 and 1.0
		m_LightColors.push_back(glm::vec3(rColor, gColor, bColor));
	}

	// Set up G-Buffer
	//textures:
	// 1. Positions (RGB)
	// 2. Color (RGB) + Specular (A)
	// 3. Normals (RGB) 
	glGenFramebuffers(1, &m_gBuffer);
	glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer);
	// - Position color buffer
	glGenTextures(1, &m_gPosition);
	glBindTexture(GL_TEXTURE_2D, m_gPosition);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_gPosition, 0);
	// - Normal color buffer
	glGenTextures(1, &m_gNormal);
	glBindTexture(GL_TEXTURE_2D, m_gNormal);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, m_gNormal, 0);
	// - Color + Specular color buffer
	glGenTextures(1, &m_gAlbedoSpec);
	glBindTexture(GL_TEXTURE_2D, m_gAlbedoSpec);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, m_gAlbedoSpec, 0);
	// - Tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
	glDrawBuffers(3, m_Attachments);

	// - Create and attach depth buffer (renderbuffer)
	glGenRenderbuffers(1, &m_rboDepth);
	glBindRenderbuffer(GL_RENDERBUFFER, m_rboDepth);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, SCR_WIDTH, SCR_HEIGHT);
	glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rboDepth);

	// - Finally check if framebuffer is complete
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "Framebuffer not complete!" << std::endl;
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_Shader->Bind();
	m_Shader->SetUniform1i("gPosition", 0);
	m_Shader->SetUniform1i("gNormal", 1);
	m_Shader->SetUniform1i("gAlbedoSpec", 2);
	m_Shader->Unbind();

	//要开启深度测试 ！！！
	GLCall(glEnable(GL_DEPTH_TEST));
}

test::TestDeferredShading::~TestDeferredShading()
{
	//资源释放
	GLCall(glDeleteVertexArrays(1, &m_QuadVAO));
	GLCall(glDeleteBuffers(1, &m_QuadVBO));
	GLCall(glDeleteVertexArrays(1, &m_CubeVAO));
	GLCall(glDeleteBuffers(1, &m_CubeVBO));
	GLCall(glDeleteFramebuffers(1, &m_gBuffer));
	GLCall(glDeleteRenderbuffers(1, &m_rboDepth));
}

void test::TestDeferredShading::OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset)
{
	m_Camera->OnProcessMouseMovement(xoffset, yoffset);
}

void test::TestDeferredShading::OnProcessInput(GLFWwindow* window, GLfloat deltaTime)
{
	m_Camera->OnProcessInput(window, deltaTime);
}

int test::TestDeferredShading::LoadImage(char const* filename)
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

int test::TestDeferredShading::LoadImage(char const* filename, bool gammaCorrection)
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

void test::TestDeferredShading::RenderQuad()
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

void test::TestDeferredShading::RenderCube()
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

void test::TestDeferredShading::OnUpdate(float deltaTime)
{
	
}

void test::TestDeferredShading::OnRender()
{
	GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	//1. 绘制到 gBuffer
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_gBuffer));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	m_GBufferShader->Bind();
	m_Proj = m_Camera->GetProjMatrix();
	m_View = m_Camera->GetViewMatrix();
	m_GBufferShader->SetUniformsMat4f("projection", m_Proj);
	m_GBufferShader->SetUniformsMat4f("view", m_View);
	for (unsigned int i = 0; i < m_ObjectPositions.size(); i++)
	{
		m_Model = glm::mat4(1.0f); //吐血，这里初始化时候一定要填数值
		m_Model = glm::translate(m_Model, m_ObjectPositions[i]);
		m_Model = glm::scale(m_Model, glm::vec3(0.1f));
		m_GBufferShader->SetUniformsMat4f("model", m_Model);
		m_RealModel->Draw(*m_GBufferShader);
	}
	m_GBufferShader->Unbind();
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	//2. 绘制gBuffer数据到 2D quad
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
	m_Shader->Bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, m_gPosition);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, m_gNormal);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, m_gAlbedoSpec);
	// Also send light relevant uniforms
	for (GLuint i = 0; i < m_LightPositions.size(); i++)
	{
		m_Shader->SetUniforms3f("lights[" + std::to_string(i) + "].Position", m_LightPositions[i]);
		m_Shader->SetUniforms3f("lights[" + std::to_string(i) + "].Color", m_LightColors[i]);

		// Update attenuation parameters and calculate radius
		const GLfloat constant = 1.0f; // Note that we don't send this to the shader, we assume it is always 1.0 (in our case)
		const GLfloat linear = 0.7f;
		const GLfloat quadratic = 1.8f;
		m_Shader->SetUniform1f("lights[" + std::to_string(i) + "].Linear", linear);
		m_Shader->SetUniform1f("lights[" + std::to_string(i) + "].Quadratic", quadratic);
		// then calculate radius of light volume/sphere
		const float maxBrightness = std::fmaxf(std::fmaxf(m_LightColors[i].r, m_LightColors[i].g), m_LightColors[i].b);
		float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
		m_Shader->SetUniform1f("lights[" + std::to_string(i) + "].Radius", radius);
	}
	m_Shader->SetUniforms3f("viewPos", m_Camera->GetPos());
	// Finally render quad
	RenderQuad();
	m_GBufferShader->Unbind();

	//2.5 正向渲染 
	glBindFramebuffer(GL_READ_FRAMEBUFFER, m_gBuffer);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // 写入默认帧缓冲
	//复制深度信息
	glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	m_BoxShader->Bind();
	m_BoxShader->SetUniformsMat4f("projection", m_Proj);
	m_BoxShader->SetUniformsMat4f("view", m_View);
	for (unsigned int i = 0; i < m_LightPositions.size(); i++)
	{
		m_Model = glm::mat4(1.0f);
		m_Model = glm::translate(m_Model, m_LightPositions[i]);
		m_Model = glm::scale(m_Model, glm::vec3(0.125f));
		m_BoxShader->SetUniformsMat4f("model", m_Model);
		m_BoxShader->SetUniforms3f("lightColor", m_LightColors[i]);
		RenderCube();
	}
	m_BoxShader->Unbind();
}

void test::TestDeferredShading::OnImGuiRender()
{
	//ImGui::SliderFloat3("ViewPos", &m_ViewPos.x, -6.0f, 6.0f);
}
