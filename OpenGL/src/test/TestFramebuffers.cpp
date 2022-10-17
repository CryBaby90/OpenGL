#include "TestFramebuffers.h"

#include <stb_image/stb_image.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>

#define DISABLETEST

test::TestFramebuffers::TestFramebuffers()
	:m_Shader(nullptr), m_ScreenShader(nullptr), m_Camera(nullptr),
	m_Model(glm::mat4(1.0f)), m_View(glm::mat4(1.0f)), m_Proj(glm::mat4(1.0f)),
	m_ViewPos(0.0f, 0.0f, -3.0f)
{
	//在上下文之后
	float cubeVertices[] = {
		// Back face
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		// Front face
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
		// Left face
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
		// Right face
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
		// Bottom face
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
		// Top face
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left        
	};

	float planeVertices[] = {
		// positions          // texture Coords 
		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
	};
	float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};

	//  cube VAO
	GLCall(glGenVertexArrays(1, &m_CubeVAO));
	GLCall(glGenBuffers(1, &m_CubeVBO));
	GLCall(glBindVertexArray(m_CubeVAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_CubeVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));

	// plane VAO
	GLCall(glGenVertexArrays(1, &m_PlaneVAO));
	GLCall(glGenBuffers(1, &m_PlaneVBO));
	GLCall(glBindVertexArray(m_PlaneVAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_PlaneVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), &planeVertices, GL_STATIC_DRAW));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));

	// screen quad VAO
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
	m_Shader = new Shader("res/shaders/Framebuffers/Vertex.Vshader", "res/shaders/Framebuffers/Fragment.Fshader");
	m_ScreenShader = new Shader("res/shaders/Framebuffers/Screen.Vshader", "res/shaders/Framebuffers/Screen.Fshader");

	//7	纹理
	
	//CubeTexture
	LoadImage(&m_CubeTextureID, "res/textures/container.jpg");
	LoadImage(&m_FloorTextureID, "res/textures/brickDiffuse.jpg");

	m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));

#ifndef DISABLETEST
#pragma region 深度模板测试
	//深度测试函数
	/*
		GL_ALWAYS	永远通过深度测试
		GL_NEVER	永远不通过深度测试
		GL_LESS	在片段深度值小于缓冲的深度值时通过测试
		GL_EQUAL	在片段深度值等于缓冲区的深度值时通过测试
		GL_LEQUAL	在片段深度值小于等于缓冲区的深度值时通过测试
		GL_GREATER	在片段深度值大于缓冲区的深度值时通过测试
		GL_NOTEQUAL	在片段深度值不等于缓冲区的深度值时通过测试
		GL_GEQUAL	在片段深度值大于等于缓冲区的深度值时通过测试*/
	GLCall(glDepthFunc(GL_LESS)); //默认 less

	//不更新深度缓冲
	//GLCall(glDepthMask(GL_FALSE));

	//打开混合
	GLCall(glEnable(GL_BLEND));

	/*	GL_ZERO							因子等于0
		GL_ONE							因子等于1
		GL_SRC_COLOR					因子等于源颜色向量C¯source
		GL_ONE_MINUS_SRC_COLOR			因子等于1−C¯source
		GL_DST_COLOR					因子等于目标颜色向量C¯destination
		GL_ONE_MINUS_DST_COLOR			因子等于1−C¯destination
		GL_SRC_ALPHA					因子等于C¯source的alpha分量
		GL_ONE_MINUS_SRC_ALPHA			因子等于1− C¯source的alpha分量
		GL_DST_ALPHA					因子等于C¯destination的alpha分量
		GL_ONE_MINUS_DST_ALPHA			因子等于1− C¯destination的alpha分量
		GL_CONSTANT_COLOR				因子等于常数颜色向量C¯constant
		GL_ONE_MINUS_CONSTANT_COLOR		因子等于1−C¯constant
		GL_CONSTANT_ALPHA				因子等于C¯constant的alpha分量
		GL_ONE_MINUS_CONSTANT_ALPHA		因子等于1− C¯constant的alpha分量*/
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	// 
	//为RGB和alpha通道分别设置不同的选项
	//GLCall(glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO));

	/*
	* 设置运算符
	* GL_FUNC_ADD：默认选项，将两个分量相加：C¯result=Src+Dst。
	  GL_FUNC_SUBTRACT：将两个分量相减： C¯result=Src−Dst。
	  GL_FUNC_REVERSE_SUBTRACT：将两个分量相减，但顺序相反：C¯result=Dst−Src。
	*/
	//GLCall(glBlendEquation(GL_FUNC_ADD));

	/*
	先绘制所有不透明的物体。
	对所有透明的物体排序。
	按顺序绘制所有透明的物体。*/

	//开启剔除
	GLCall(glEnable(GL_CULL_FACE));
	/*
	  GL_BACK：只剔除背向面。
	  GL_FRONT：只剔除正向面。
	  GL_FRONT_AND_BACK：剔除正向面和背向面。
	*/
	GLCall(glCullFace(GL_FRONT));

	/*
	* GL_CCW 逆时针
	* GL_CW  顺时针
	*/
	GLCall(glFrontFace(GL_CCW));
#pragma endregion
#endif 

	/*
	* 完整帧缓冲：
	* 1：附加至少一个缓冲（颜色、深度或模板缓冲）。
	* 2：至少有一个颜色附件(Attachment)。
	* 3：所有的附件都必须是完整的（保留了内存）。
	* 4：每个缓冲都应该有相同的样本数。
	*/
	//1	创建帧缓冲
	GLCall(glGenFramebuffers(1, &m_FBO));
	/*
	* 绑定帧缓冲
	* 也可以分别绑定读、写
	* GL_READ_FRAMEBUFFER 将会使用在所有像是glReadPixels的读取操作中
	* GL_DRAW_FRAMEBUFFER 将会被用作渲染、清除等写入操作的目标
	*/
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FBO));

	//2	纹理附件 (Attachment)
	GLCall(glGenTextures(1, &m_TextureColorbuffer));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureColorbuffer));
	/*	target：帧缓冲的目标（绘制、读取或者两者皆有）
		attachment：我们想要附加的附件类型。当前我们正在附加一个颜色附件。注意最后的0意味着我们可以附加多个颜色附件。我们将在之后的教程中提到。
		textarget：你希望附加的纹理类型
		texture：要附加的纹理本身
		level：多级渐远纹理的级别。我们将它保留为0。*/
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureColorbuffer, 0));
	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 800, 600, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_TextureColorbuffer, 0));

	/* 不需要采样缓冲的时候，渲染缓冲对象是更好的选择
	//加入深度缓冲
	//GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 800, 600, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL));
	//GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_CubeTextureID, 0));

	//加入深度缓冲和模板缓冲 ： GL_DEPTH24_STENCIL8
	GLCall(glTexImage2D(
		GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, 800, 600, 0,
		GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
	));
	GLCall(glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, m_CubeTextureID, 0));
	*/

	//创建渲染缓冲对象
	GLCall(glGenRenderbuffers(1, &m_RBO));
	GLCall(glBindRenderbuffer(GL_RENDERBUFFER, m_RBO));
	/*
	  由于渲染缓冲对象通常都是只写的，它们会经常用于深度和模板附件，因为大部分时间我们都不需要从深度和模板缓冲中读取值，
	  只关心深度和模板测试。我们需要深度和模板值用于测试，但不需要对它们进行采样，所以渲染缓冲对象非常适合它们。
	  当我们不需要从这些缓冲中采样的时候，通常都会选择渲染缓冲对象，因为它会更优化一点。
	  创建一个深度和模板渲染缓冲对象可以通过调用glRenderbufferStorage函数来完成：
	*/
	GLCall(glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, 800, 600));
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
	//GLCall(glDeleteFramebuffers(1, &m_FBO));
}

test::TestFramebuffers::~TestFramebuffers()
{
	//资源释放
	delete m_Shader;
	delete m_ScreenShader;
	GLCall(glDeleteVertexArrays(1, &m_CubeVAO));
	GLCall(glDeleteBuffers(1, &m_CubeVBO));

	GLCall(glDeleteVertexArrays(1, &m_PlaneVAO));
	GLCall(glDeleteBuffers(1, &m_PlaneVAO));

	GLCall(glDeleteVertexArrays(1, &m_QuadVAO));
	GLCall(glDeleteBuffers(1, &m_QuadVBO));
}

void test::TestFramebuffers::OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset)
{
	m_Camera->OnProcessMouseMovement(xoffset, yoffset);
}

void test::TestFramebuffers::LoadImage(GLuint* textureID, char const* filename)
{
	GLCall(glGenTextures(1, textureID));
	GLCall(glBindTexture(GL_TEXTURE_2D, *textureID));
	//加载并生成纹理
	GLint width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
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

		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, textureData));
		GLCall(glGenerateMipmap(GL_TEXTURE_2D));

		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT)); 
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	}
	else
	{
		std::cout << "Failed to load texture1" << std::endl;
	}
	stbi_image_free(textureData);
}

void test::TestFramebuffers::OnUpdate(float deltaTime)
{
	/*m_Model = glm::mat4(1.0f);
	m_Model = glm::rotate(m_Model, deltaTime * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

	m_View = glm::mat4(1.0f);
	m_View = glm::translate(m_View, m_ViewPos);*/
}

void test::TestFramebuffers::OnRender()
{
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FBO));
	GLCall(glEnable(GL_DEPTH_TEST));
	GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	//  绘制物体
	//为图片设置插槽（纹理单元）
	GLCall(glActiveTexture(GL_TEXTURE0));

	m_Model = glm::mat4(1.0f);
	m_View = m_Camera->GetViewMatrix();
	m_Proj = m_Camera->GetProjMatrix();

	//先use shader
	m_Shader->Bind();
	//给shader里的变量指定插槽
	m_Shader->SetUniform1i("texture1", 0);
	m_Shader->SetUniformsMat4f("view", m_View);
	m_Shader->SetUniformsMat4f("proj", m_Proj);

	//cube
	m_Model = glm::translate(m_Model, glm::vec3(-1.0f, 0.0f, -1.0f));
	m_Shader->SetUniformsMat4f("model", m_Model);
	GLCall(glBindVertexArray(m_CubeVAO));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_CubeTextureID));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36)); //没有设置index pos  暂时用glDrawArrays
	m_Model = glm::mat4(1.0f);
	m_Model = glm::translate(m_Model, glm::vec3(2.0f, 0.0f, 0.0f));
	m_Shader->SetUniformsMat4f("model", m_Model);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));

	// floor
	GLCall(glBindVertexArray(m_PlaneVAO));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_FloorTextureID));
	m_Shader->SetUniformsMat4f("model", glm::mat4(1.0f));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
	GLCall(glBindVertexArray(0));

	//默认帧缓冲
	GLCall(glBindFramebuffer(GL_FRAMEBUFFER, 0));
	GLCall(glDisable(GL_DEPTH_TEST));
	GLCall(glClearColor(1.0f, 1.0f, 1.0f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT));

	m_ScreenShader->Bind();
	m_Shader->SetUniform1i("screenTexture", 0);
	GLCall(glBindVertexArray(m_QuadVAO));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureColorbuffer));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
}

void test::TestFramebuffers::OnImGuiRender()
{
	//ImGui::SliderFloat3("ViewPos", &m_ViewPos.x, -6.0f, 6.0f);
}
