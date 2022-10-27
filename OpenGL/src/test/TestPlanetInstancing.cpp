#include "TestPlanetInstancing.h"

#include <stb_image/stb_image.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>
#include <sstream>

test::TestPlanetInstancing::TestPlanetInstancing()
	:m_Shader(nullptr), m_Camera(nullptr), m_Planet(nullptr), m_Rock(nullptr),
	m_ModelMatrices(nullptr)
{
	//在上下文之后
	InitModelMatrices();

	//6  着色器程序
	m_Shader = std::make_unique<Shader>("res/shaders/PlanetInstancing/Vertex.Vshader", "res/shaders/PlanetInstancing/Fragment.Fshader");
	m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 55.0f));

	m_Planet = std::make_unique<Model>("res/obj/Planet/planet.obj");
	m_Rock = std::make_unique<Model>("res/obj/Planet/Rock/rock.obj");

	GLCall(glEnable(GL_DEPTH_TEST));
}

test::TestPlanetInstancing::~TestPlanetInstancing()
{
	//资源释放
	GLCall(glDeleteVertexArrays(1, &m_CubeVAO));
	GLCall(glDeleteBuffers(1, &m_CubeVBO));

	delete m_ModelMatrices;
}

void test::TestPlanetInstancing::OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset)
{
	m_Camera->OnProcessMouseMovement(xoffset, yoffset);
}

void test::TestPlanetInstancing::OnProcessInput(GLFWwindow* window, GLfloat deltaTime)
{
	m_Camera->OnProcessInput(window, deltaTime);
}

int test::TestPlanetInstancing::LoadImage(char const* filename)
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

int test::TestPlanetInstancing::LoadCubemap(std::vector<std::string> faces)
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

void test::TestPlanetInstancing::InitModelMatrices()
{
	m_ModelMatrices = new glm::mat4[m_Amount];
	srand(glfwGetTime()); // 初始化随机种子    
	float radius = 50.0;
	float offset = 2.5f;
	for (unsigned int i = 0; i < m_Amount; i++)
	{
		glm::mat4 model = glm::mat4(1.0f);
		// 1. 位移：分布在半径为 'radius' 的圆形上，偏移的范围是 [-offset, offset]
		float angle = (float)i / (float)m_Amount * 360.0f;
		float displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float x = sin(angle) * radius + displacement;
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float y = displacement * 0.4f; // 让行星带的高度比x和z的宽度要小
		displacement = (rand() % (int)(2 * offset * 100)) / 100.0f - offset;
		float z = cos(angle) * radius + displacement;
		model = glm::translate(model, glm::vec3(x, y, z));

		// 2. 缩放：在 0.05 和 0.25f 之间缩放
		float scale = (rand() % 20) / 100.0f + 0.05;
		model = glm::scale(model, glm::vec3(scale));

		// 3. 旋转：绕着一个（半）随机选择的旋转轴向量进行随机的旋转
		float rotAngle = (rand() % 360);
		model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

		// 4. 添加到矩阵的数组中
		m_ModelMatrices[i] = model;
	}
}

void test::TestPlanetInstancing::OnUpdate(float deltaTime)
{
	
}

void test::TestPlanetInstancing::OnRender()
{
	GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	m_Shader->Bind();
	glm::mat4 projection = m_Camera->GetProjMatrix();
	glm::mat4 view = m_Camera->GetViewMatrix();;
	m_Shader->SetUniformsMat4f("projection", projection);
	m_Shader->SetUniformsMat4f("view", view);

	glm::mat4 model = glm::mat4(1.0f);
	model = glm::translate(model, glm::vec3(0.0f, -3.0f, 0.0f));
	model = glm::scale(model, glm::vec3(4.0f, 4.0f, 4.0f));
	m_Shader->SetUniformsMat4f("model", model);
    m_Planet->Draw(*m_Shader);

	// 绘制小行星
	for (unsigned int i = 0; i < m_Amount; i++)
	{
		m_Shader->SetUniformsMat4f("model", m_ModelMatrices[i]);
		m_Rock->Draw(*m_Shader);
	}
}

void test::TestPlanetInstancing::OnImGuiRender()
{
	//ImGui::SliderFloat3("ViewPos", &m_ViewPos.x, -6.0f, 6.0f);
}
