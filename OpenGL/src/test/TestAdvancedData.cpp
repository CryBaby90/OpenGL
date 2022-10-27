﻿#include "TestAdvancedData.h"

#include <stb_image/stb_image.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>

#define DISABLETEST

test::TestAdvancedData::TestAdvancedData()
	:m_Shader(nullptr), m_Camera(nullptr),
	m_Model(glm::mat4(1.0f)), m_View(glm::mat4(1.0f)), m_Proj(glm::mat4(1.0f)),
	m_ViewPos(0.0f, 0.0f, -3.0f)
{
	//在上下文之后
	float cubeVertices[] = {
	// positions        
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
	float cubeNormals[] = {
		 //Normal
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,
		 0.0f,  0.0f, -1.0f,

		 0.0f,  0.0f, 1.0f,
		 0.0f,  0.0f, 1.0f,
		 0.0f,  0.0f, 1.0f,
		 0.0f,  0.0f, 1.0f,
		0.0f,  0.0f, 1.0f,
		0.0f,  0.0f, 1.0f,

		0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,
		-1.0f,  0.0f,  0.0f,

		 1.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,
		 1.0f,  0.0f,  0.0f,

		0.0f, -1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,
		 0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,
		0.0f, -1.0f,  0.0f,

		0.0f,  1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,
		 0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f,
		0.0f,  1.0f,  0.0f
	};
	float skyboxVertices[] = {
		// positions          
		-1.0f,  1.0f, -1.0f,
		-1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f, -1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,

		-1.0f, -1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f,
		-1.0f, -1.0f,  1.0f,

		-1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f, -1.0f,
		 1.0f,  1.0f,  1.0f,
		 1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f,  1.0f,
		-1.0f,  1.0f, -1.0f,

		-1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f, -1.0f,
		 1.0f, -1.0f, -1.0f,
		-1.0f, -1.0f,  1.0f,
		 1.0f, -1.0f,  1.0f
	};

	std::vector<std::string> faces
	{
		"res/textures/skybox/right.jpg",
		"res/textures/skybox/left.jpg",
		"res/textures/skybox/top.jpg",
		"res/textures/skybox/bottom.jpg",
		"res/textures/skybox/front.jpg",
		"res/textures/skybox/back.jpg"
	};

	//  cube VAO
	GLCall(glGenVertexArrays(1, &m_CubeVAO));
	GLCall(glGenBuffers(1, &m_CubeVBO));
	GLCall(glBindVertexArray(m_CubeVAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_CubeVBO));
	//// 获取指针  
	//void* ptr = glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY);
	//// 复制数据到内存
	//memcpy(ptr, cubeVertices, sizeof(cubeVertices));
	//// 记得告诉OpenGL我们不再需要这个指针了
	//glUnmapBuffer(GL_ARRAY_BUFFER);  //有报错

	//绑定bufferdata 预留大小 sizeof(cubeVertices) + sizeof(cubeNormals) 数据为：NULL
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices) + sizeof(cubeNormals), NULL, GL_STATIC_DRAW));
	GLCall(glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(cubeVertices), &cubeVertices));
	GLCall(glBufferSubData(GL_ARRAY_BUFFER, sizeof(cubeVertices), sizeof(cubeNormals), &cubeNormals));

	/*
	void glCopyBufferSubData(GLenum readtarget, GLenum writetarget, GLintptr readoffset,
		GLintptr writeoffset, GLsizeiptr size);
	GLuint vbo1 = 0;
	GLuint	vbo2 = 0;
	glBindBuffer(GL_ARRAY_BUFFER, vbo1);
	glBindBuffer(GL_COPY_WRITE_BUFFER, vbo2);
	glCopyBufferSubData(GL_ARRAY_BUFFER, GL_COPY_WRITE_BUFFER, 0, 0, sizeof(vertexData));*/

	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sizeof(cubeVertices))));
	GLCall(glBindVertexArray(0));

	// cubemps VAO
	GLCall(glGenVertexArrays(1, &m_CubemapsVAO));
	GLCall(glGenBuffers(1, &m_CubemapsVBO));
	GLCall(glBindVertexArray(m_CubemapsVAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_CubemapsVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), skyboxVertices, GL_STATIC_DRAW));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
	GLCall(glBindVertexArray(0));

	//6  着色器程序
	m_Shader = std::make_unique<Shader>("res/shaders/AdvancedData/Vertex.Vshader", "res/shaders/AdvancedData/Fragment.Fshader");
	m_CubemapsShader = std::make_unique<Shader>("res/shaders/AdvancedData/Cubemaps.Vshader", "res/shaders/AdvancedData/Cubemaps.Fshader");

	//7	纹理
	//CubeTexture
	m_CubeTextureID = LoadImage("res/textures/container.jpg");
	//LoadImage(&m_CubeTextureID, "res/textures/container.jpg");
	m_CubemapTextureID = LoadCubemap(faces);

	m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));

	m_Shader->Bind();
	//给shader里的变量指定插槽
	//m_Shader->SetUniform1i("texture1", 0);

	m_CubemapsShader->Bind();
	m_CubemapsShader->SetUniform1i("skybox", 0);

	GLCall(glEnable(GL_DEPTH_TEST));
}

test::TestAdvancedData::~TestAdvancedData()
{
	//资源释放
	GLCall(glDeleteVertexArrays(1, &m_CubeVAO));
	GLCall(glDeleteBuffers(1, &m_CubeVBO));
	GLCall(glDeleteVertexArrays(1, &m_CubemapsVAO));
	GLCall(glDeleteBuffers(1, &m_CubemapsVBO));
}

void test::TestAdvancedData::OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset)
{
	m_Camera->OnProcessMouseMovement(xoffset, yoffset);
}

void test::TestAdvancedData::OnProcessInput(GLFWwindow* window, GLfloat deltaTime)
{
	m_Camera->OnProcessInput(window, deltaTime);
}

int test::TestAdvancedData::LoadImage(char const* filename)
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

int test::TestAdvancedData::LoadCubemap(std::vector<std::string> faces)
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

void test::TestAdvancedData::OnUpdate(float deltaTime)
{
	/*m_Model = glm::mat4(1.0f);
	m_Model = glm::rotate(m_Model, deltaTime * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

	m_View = glm::mat4(1.0f);
	m_View = glm::translate(m_View, m_ViewPos);*/
}

void test::TestAdvancedData::OnRender()
{
	GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));


	m_Shader->Bind();
	m_Model = glm::mat4(1.0f);
	m_View = m_Camera->GetViewMatrix();
	m_Proj = m_Camera->GetProjMatrix(); 
	m_Shader->SetUniformsMat4f("model", m_Model);
	m_Shader->SetUniformsMat4f("view", m_View);
	m_Shader->SetUniformsMat4f("proj", m_Proj);
	m_Shader->SetUniforms3f("cameraPos", m_Camera->GetPos());
	// cubes
	GLCall(glBindVertexArray(m_CubeVAO));
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_CubeTextureID));
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubemapTextureID)); //反射 绑定CubemapTexture
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
	GLCall(glBindVertexArray(0));

	// draw skybox as last
	GLCall(glDepthFunc(GL_LEQUAL));  // change depth function so depth test passes when values are equal to depth buffer's content
	m_CubemapsShader->Bind();
	m_View = glm::mat4(glm::mat3(m_Camera->GetViewMatrix())); // remove translation from the view matrix
	m_CubemapsShader->SetUniformsMat4f("view", m_View);
	m_CubemapsShader->SetUniformsMat4f("proj", m_Proj);
	// skybox cube
	GLCall(glBindVertexArray(m_CubemapsVAO));
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubemapTextureID));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));
	GLCall(glBindVertexArray(0));
	GLCall(glDepthFunc(GL_LESS));
}

void test::TestAdvancedData::OnImGuiRender()
{
	//ImGui::SliderFloat3("ViewPos", &m_ViewPos.x, -6.0f, 6.0f);
}