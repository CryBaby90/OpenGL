#include "TestAntiAliasing.h"
#include "Global.h"

#include <stb_image/stb_image.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <sstream>

test::TestAntiAliasing::TestAntiAliasing()
	:m_Shader(nullptr), m_ScreenShader(nullptr), m_Camera(nullptr)
{
	//在上下文之后
	GLfloat cubeVertices[] = {
		// Positions       
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
		-0.5f,  0.5f, -0.5f
	};
	float quadVertices[] = {   // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	//  VAO
	GLCall(glGenVertexArrays(1, &m_CubeVAO));
	GLCall(glGenBuffers(1, &m_CubeVBO));
	GLCall(glBindVertexArray(m_CubeVAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_CubeVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
	GLCall(glBindVertexArray(0));

	GLCall(glGenVertexArrays(1, &m_QuadVAO));
	GLCall(glGenBuffers(1, &m_QuadVBO));
	GLCall(glBindVertexArray(m_QuadVAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_QuadVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float))));
	GLCall(glBindVertexArray(0));

	//6  着色器程序
	m_Shader = std::make_unique<Shader>("res/shaders/AntiAliasing/Vertex.Vshader", "res/shaders/AntiAliasing/Fragment.Fshader");
	m_ScreenShader = std::make_unique<Shader>("res/shaders/AntiAliasing/Screen.Vshader", "res/shaders/AntiAliasing/Screen.Fshader");
	m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));

	//1	创建帧缓冲
	GLCall(glGenFramebuffers(1, &m_FBO));
	/*
	* 绑定帧缓冲
	* 也可以分别绑定读、写
	* GL_READ_FRAMEBUFFER 将会使用在所有像是glReadPixels的读取操作中
	* GL_DRAW_FRAMEBUFFER 将会被用作渲染、清除等写入操作的目标
	*/
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FBO));

	//2: 纹理附件 (Attachment)
	//create a multisampled color attachment texture
	unsigned int textureColorBufferMultiSampled;
	GLCall(glGenTextures(1, &textureColorBufferMultiSampled));
	GLCall(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled));
	GLCall(glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGB, SCR_WIDTH, SCR_HEIGHT, GL_TRUE));
	GLCall(glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0));
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled, 0));

	//创建渲染缓冲对象
	GLCall(glGenRenderbuffers(1, &m_RBO));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, m_RBO));
	/*
	  由于渲染缓冲对象通常都是只写的，它们会经常用于深度和模板附件，因为大部分时间我们都不需要从深度和模板缓冲中读取值，
	  只关心深度和模板测试。我们需要深度和模板值用于测试，但不需要对它们进行采样，所以渲染缓冲对象非常适合它们。
	  当我们不需要从这些缓冲中采样的时候，通常都会选择渲染缓冲对象，因为它会更优化一点。
	  创建一个深度和模板渲染缓冲对象可以通过调用glRenderbufferStorage函数来完成：
	*/
	GLCall(glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, SCR_WIDTH, SCR_HEIGHT));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
	// 附加这个渲染缓冲对象
	GLCall(glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, m_RBO));

	/*
	  总结
	  纹理附件和渲染缓冲对象选择：
	  如果你不需要从一个缓冲中采样数据，那么对这个缓冲使用渲染缓冲对象会是明智的选择。
	  如果你需要从缓冲中采样颜色或深度值等数据，那么你应该选择纹理附件
	*/

	//检测帧缓冲是否完整
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	}
	//要保证所有的渲染操作在主窗口中有视觉效果，我们需要再次激活默认帧缓冲，将它绑定到0。
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	// configure second post-processing framebuffer
	GLCall(glGenFramebuffers(1, &m_IntermediateFBO));
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_IntermediateFBO));
	// create a color attachment texture
	GLCall(glGenTextures(1, &m_TextureColorbuffer));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureColorbuffer));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, SCR_WIDTH, SCR_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	// we only need a color buffer
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureColorbuffer, 0));	

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << std::endl;
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));

	m_ScreenShader->Bind();
	GLCall(glActiveTexture(GL_TEXTURE0));
	m_ScreenShader->SetUniform1i("screenTexture", 0);
	m_ScreenShader->Unbind();
}

test::TestAntiAliasing::~TestAntiAliasing()
{
	//资源释放
	GLCall(glDeleteVertexArrays(1, &m_CubeVAO));
	GLCall(glDeleteBuffers(1, &m_CubeVBO));

	GLCall(glDeleteVertexArrays(1, &m_QuadVAO));
	GLCall(glDeleteBuffers(1, &m_QuadVBO));

	GLCall(glDeleteFramebuffers(1, &m_FBO));
	GLCall(glDeleteRenderbuffers(1, &m_RBO));

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, 0));
}

void test::TestAntiAliasing::OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset)
{
	m_Camera->OnProcessMouseMovement(xoffset, yoffset);
}

void test::TestAntiAliasing::OnProcessInput(GLFWwindow* window, GLfloat deltaTime)
{
	m_Camera->OnProcessInput(window, deltaTime);
}

int test::TestAntiAliasing::LoadImage(char const* filename)
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

void test::TestAntiAliasing::OnUpdate(float deltaTime)
{
	
}

void test::TestAntiAliasing::OnRender()
{
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FBO));
	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	m_Shader->Bind();
	m_Model = glm::mat4(1.0f);
	m_Model = glm::rotate(m_Model, glm::radians(90.0f), glm::vec3(0.0f, 0.5f, 1.0f));
	m_View = m_Camera->GetViewMatrix();
	m_Proj = m_Camera->GetProjMatrix();
	m_Shader->SetUniformsMat4f("model", m_Model);
	m_Shader->SetUniformsMat4f("view", m_View);
	m_Shader->SetUniformsMat4f("proj", m_Proj);
	GLCall(glBindVertexArray(m_CubeVAO));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));

	// 2. now blit multisampled buffer(s) to normal colorbuffer of intermediate FBO. Image is stored in screenTexture
	GLCall(glBindFramebuffer(GL_READ_FRAMEBUFFER, m_FBO));
	GLCall(glBindFramebuffer(GL_DRAW_FRAMEBUFFER, m_IntermediateFBO));
	GLCall(glBlitFramebuffer(0, 0, SCR_WIDTH, SCR_HEIGHT, 0, 0, SCR_WIDTH, SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST));

	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GLCall(glDisable(GL_DEPTH_TEST));
	GLCall(glClearColor(1.0f, 1.0f, 1.0f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT));

	m_ScreenShader->Bind();
	GLCall(glBindVertexArray(m_QuadVAO));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureColorbuffer));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
}

void test::TestAntiAliasing::OnImGuiRender()
{
	//ImGui::SliderFloat3("ViewPos", &m_ViewPos.x, -6.0f, 6.0f);
}
