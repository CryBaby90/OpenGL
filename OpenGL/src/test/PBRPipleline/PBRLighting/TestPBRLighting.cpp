#include "../test/PBRPipleline/PBRLighting/TesPBRLighting.h"
#include "Global.h"

#include <stb_image/stb_image.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <sstream>
#include <random>

test::TesPBRLighting::TesPBRLighting()
	:m_Shader(nullptr), m_Camera(nullptr), m_RealModel(nullptr)
{
	/*
	* 延迟渲染确实带来一些缺点：大内存开销，没有MSAA和混合(仍需要正向渲染的配合)。
	* 当你有一个很小的场景并且没有很多的光源时候，
	* 延迟渲染并不一定会更快一点，甚至有些时候由于开销超过了它的优点还会更慢。
	*/

	m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 20.0f));

	// 着色器程序
	m_Shader = std::make_unique<Shader>("res/shaders/PBRPipleline/PBRLighting/Vertex.Vshader", "res/shaders/PBRPipleline/PBRLighting/Fragment.Fshader");

	m_Shader->Bind();
	m_Shader->SetUniforms3f("albedo", glm::vec3(0.5f, 0.0f, 0.0f));
	m_Shader->SetUniform1f("ao", 1.0f);
	m_Shader->Unbind();

	//要开启深度测试 ！！！
	GLCall(glEnable(GL_DEPTH_TEST));
}

test::TesPBRLighting::~TesPBRLighting()
{
	//资源释放
	GLCall(glDeleteVertexArrays(1, &m_QuadVAO));
	GLCall(glDeleteBuffers(1, &m_QuadVBO));
	GLCall(glDeleteVertexArrays(1, &m_CubeVAO));
	GLCall(glDeleteBuffers(1, &m_CubeVBO));
	GLCall(glDeleteVertexArrays(1, &m_SphereVAO));
}

void test::TesPBRLighting::OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset)
{
	m_Camera->OnProcessMouseMovement(xoffset, yoffset);
}

void test::TesPBRLighting::OnProcessInput(GLFWwindow* window, GLfloat deltaTime)
{
	m_Camera->OnProcessInput(window, deltaTime);
}

int test::TesPBRLighting::LoadImage(char const* filename)
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

int test::TesPBRLighting::LoadImage(char const* filename, bool gammaCorrection)
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

void test::TesPBRLighting::RenderQuad()
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

void test::TesPBRLighting::RenderCube()
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

void test::TesPBRLighting::RenderSphere()
{
	if (m_SphereVAO == 0)
	{
		glGenVertexArrays(1, &m_SphereVAO);

		unsigned int vbo, ebo;
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;

		const unsigned int X_SEGMENTS = 64;
		const unsigned int Y_SEGMENTS = 64;
		const float PI = 3.14159265359f;
		for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
		{
			for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
			{
				float xSegment = (float)x / (float)X_SEGMENTS;
				float ySegment = (float)y / (float)Y_SEGMENTS;
				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				float yPos = std::cos(ySegment * PI);
				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

				positions.push_back(glm::vec3(xPos, yPos, zPos));
				uv.push_back(glm::vec2(xSegment, ySegment));
				normals.push_back(glm::vec3(xPos, yPos, zPos));
			}
		}

		bool oddRow = false;
		for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
		{
			if (!oddRow) // even rows: y == 0, y == 2; and so on
			{
				for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
				{
					indices.push_back(y * (X_SEGMENTS + 1) + x);
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				}
			}
			else
			{
				for (int x = X_SEGMENTS; x >= 0; --x)
				{
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					indices.push_back(y * (X_SEGMENTS + 1) + x);
				}
			}
			oddRow = !oddRow;
		}
		indexCount = static_cast<unsigned int>(indices.size());

		std::vector<float> data;
		for (unsigned int i = 0; i < positions.size(); ++i)
		{
			data.push_back(positions[i].x);
			data.push_back(positions[i].y);
			data.push_back(positions[i].z);
			if (normals.size() > 0)
			{
				data.push_back(normals[i].x);
				data.push_back(normals[i].y);
				data.push_back(normals[i].z);
			}
			if (uv.size() > 0)
			{
				data.push_back(uv[i].x);
				data.push_back(uv[i].y);
			}
		}
		glBindVertexArray(m_SphereVAO);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
		unsigned int stride = (3 + 2 + 3) * sizeof(float);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
		glEnableVertexAttribArray(1);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(2);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
	}

	glBindVertexArray(m_SphereVAO);
	glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}

void test::TesPBRLighting::OnUpdate(float deltaTime)
{
	
}

void test::TesPBRLighting::OnRender()
{
	GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	m_Shader->Bind();
	m_Proj = m_Camera->GetProjMatrix();
	m_View = m_Camera->GetViewMatrix();
	m_Shader->SetUniformsMat4f("projection", m_Proj);
	m_Shader->SetUniformsMat4f("view", m_View);
	m_Shader->SetUniforms3f("camPos", m_Camera->GetPos());
	glm::mat4 model = glm::mat4(1.0f);
	for (int row = 0; row < nrRows; ++row)
	{
		m_Shader->SetUniform1f("metallic", (float)row / (float)nrRows);
		for (int col = 0; col < nrColumns; ++col)
		{
			// we clamp the roughness to 0.05 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
			// on direct lighting.
			m_Shader->SetUniform1f("roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

			model = glm::mat4(1.0f);
			model = glm::translate(model, glm::vec3(
				(col - (nrColumns / 2)) * spacing,
				(row - (nrRows / 2)) * spacing,
				0.0f
			));
			m_Shader->SetUniformsMat4f("model", model);
			RenderSphere();
		}
	}

	//灯光
	for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
	{
		glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
		newPos = lightPositions[i];
		m_Shader->SetUniforms3f("lightPositions[" + std::to_string(i) + "]", newPos);
		m_Shader->SetUniforms3f("lightColors[" + std::to_string(i) + "]", lightColors[i]);

		model = glm::mat4(1.0f);
		model = glm::translate(model, newPos);
		model = glm::scale(model, glm::vec3(0.5f));
		m_Shader->SetUniformsMat4f("model", model);
		RenderCube();
	}
	m_Shader->Unbind();
}

void test::TesPBRLighting::OnImGuiRender()
{
	//ImGui::SliderFloat3("ViewPos", &m_ViewPos.x, -6.0f, 6.0f);
}
