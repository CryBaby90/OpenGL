#include "TestMatrix.h"
#include "GLCall.h"
#include "Shader.h"

#include <stb_image/stb_image.h>
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <cmath>

test::TestMatrix::TestMatrix()
	:m_Shader(nullptr), m_MixValue(0.2f), m_Trans(glm::mat4(1.0f)), 
	m_Rotate(0.0f), m_Scale(0.0f, 0.0f, 1.0f)
{
	//��������֮��
	GLfloat vertices[] = {
		//     ---- λ�� ----          - �������� -
	 0.5f,  0.5f, 0.0f,					1.0f, 1.0f,   // ����
	 0.5f, -0.5f, 0.0f,				    1.0f, 0.0f,   // ����
	-0.5f, -0.5f, 0.0f,					0.0f, 0.0f,   // ����
	-0.5f,  0.5f, 0.0f,					0.0f, 1.0f    // ����
	};

	GLuint indices[] = {
		// ע��������0��ʼ! 
		// ����������(0,1,2,3)���Ƕ�������vertices���±꣬
		// �����������±����������ϳɾ���
		0, 1, 3, // ��һ��������
		1, 2, 3  // �ڶ���������
	};

	//  ����VAO, VBO, EBO
	GLCall(glGenVertexArrays(1, &m_VAO));
	GLCall(glGenBuffers(1, &m_VBO));//����VBO
	GLCall(glGenBuffers(1, &m_EBO));//����EBO = IBO:index buffer

	//1  ��VAO�������
	GLCall(glBindVertexArray(m_VAO));

	//2  ��VBO���������飩�������й�OpenGLʹ��
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));//���㻺����󣬶��㻺���� GL_ARRAY_BUFFER��һ������Ŀ��
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));//�������
																					  //GL_STATIC_DRAW �����ݲ���򼸺�����ı䡣
																					  //GL_DYNAMIC_DRAW�����ݻᱻ�ı�ܶࡣ
																					  //GL_STREAM_DRAW ������ÿ�λ���ʱ����ı䡣

	//3  ��EBO���������飩��һ�����������У���OpenGLʹ�� 
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

	//4  ���ö�������ָ��
	//�������� layout(location = 0)
	//�������ԵĴ�С
	//����ָ�����ݵ�����
	//�Ƿ�ϣ�����ݱ���׼��(Normalize)�������������ΪGL_TRUE���������ݶ��ᱻӳ�䵽0�������з�����signed������-1����1֮�䡣���ǰ�������ΪGL_FALSE��
	//����(Stride)
	//λ�������ڻ�������ʼλ�õ�ƫ����(Offset)������λ������������Ŀ�ͷ������������0
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(0));

	GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));
	GLCall(glEnableVertexAttribArray(1));

	//  ���VAO  ֮ǰ��Ҫ���VBO EBO ����Ҫ���°�
	GLCall(glBindVertexArray(0));

	//6  ��ɫ������
	m_Shader = new Shader("res/shaders/Matrix/Vertex.Vshader", "res/shaders/Matrix/Fragment.Fshader");

	//7	����
	GLCall(glGenTextures(1, &m_TextureID1));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureID1));
	// Ϊ��ǰ�󶨵������������û��ơ����˷�ʽ
	//GL_REPEAT	��������Ĭ����Ϊ���ظ�����ͼ��
	//GL_MIRRORED_REPEAT	��GL_REPEATһ������ÿ���ظ�ͼƬ�Ǿ�����õ�
	//GL_CLAMP_TO_EDGE	��������ᱻԼ����0��1֮�䣬�����Ĳ��ֻ��ظ���������ı�Ե������һ�ֱ�Ե�������Ч����
	//GL_CLAMP_TO_BORDER	����������Ϊ�û�ָ���ı�Ե��ɫ��
	//�����GL_CLAMP_TO_BORDER ��Ҫָ����Ե��ɫ��float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	//											glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	//GL_NEAREST				�ڽ����� ��ѡ�����ĵ���ӽ�����������Ǹ�����
	//GL_LINEAR					���Թ��� ��������������긽�����������أ������һ����ֵ�����Ƴ���Щ��������֮�����ɫ
	//GL_NEAREST_MIPMAP_NEAREST	ʹ�����ڽ��Ķ༶��Զ������ƥ�����ش�С����ʹ���ڽ���ֵ������������
	//GL_LINEAR_MIPMAP_NEAREST	ʹ�����ڽ��Ķ༶��Զ�������𣬲�ʹ�����Բ�ֵ���в���
	//GL_NEAREST_MIPMAP_LINEAR	��������ƥ�����ش�С�Ķ༶��Զ����֮��������Բ�ֵ��ʹ���ڽ���ֵ���в���
	//GL_LINEAR_MIPMAP_LINEAR	�������ڽ��Ķ༶��Զ����֮��ʹ�����Բ�ֵ����ʹ�����Բ�ֵ���в���
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	//���ز���������
	GLint width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* textureData = stbi_load("res/textures/container.jpg", &width, &height, &nrChannels, 0);
	if (textureData)
	{
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData));
		GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	}
	else
	{
		std::cout << "Failed to load texture1" << std::endl;
	}
	stbi_image_free(textureData);

	GLCall(glGenTextures(1, &m_TextureID2));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureID2));
	// Ϊ��ǰ�󶨵������������û��ơ����˷�ʽ
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	textureData = stbi_load("res/textures/awesomeface.png", &width, &height, &nrChannels, 0);
	if (textureData)
	{
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, textureData));
		GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	}
	else
	{
		std::cout << "Failed to load texture1" << std::endl;
	}
	stbi_image_free(textureData);

	glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);
	glm::mat4 trans = glm::mat4(1.0f);
	trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f));
	vec = trans * vec;
	std::cout << "x" << vec.x << std::endl;
	std::cout << "y" << vec.y << std::endl;
	std::cout << "z" << vec.z << std::endl;

	//��ס��ʵ�ʵı任˳��Ӧ�����Ķ�˳���෴��ʵ�ʵı任�������ţ���Ӧ����ת�������λ�Ƶġ�
	m_Trans = glm::rotate(m_Trans, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
	m_Trans = glm::translate(m_Trans, glm::vec3(1.0f, 0.0f, 0.0f));	
}

test::TestMatrix::~TestMatrix()
{
	//��Դ�ͷ�
	delete m_Shader;
	GLCall(glDeleteVertexArrays(1, &m_VAO));
	GLCall(glDeleteBuffers(1, &m_VBO));
	GLCall(glDeleteBuffers(1, &m_EBO));
}

void test::TestMatrix::OnUpdate(float deltaTime)
{
	//m_Trans = glm::rotate(m_Trans, glm::radians(m_Rotate), m_Scale);
	//m_Trans = glm::scale(m_Trans, m_Scale);
}

void test::TestMatrix::OnRender()
{
	m_Trans = glm::mat4(1.0f);
	m_Trans = glm::rotate(m_Trans, (float)glfwGetTime(), glm::vec3(0.0f, 0.0f, 1.0f));
	m_Trans = glm::translate(m_Trans, glm::vec3(0.5f, -0.5f, 0.0f));
	

	//  ��������
	//ΪͼƬ���ò�ۣ�������Ԫ��
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureID1));
	GLCall(glActiveTexture(GL_TEXTURE1));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureID2));
	//��use shader
	m_Shader->Bind();
	//��shader��ı���ָ�����
	m_Shader->SetUniform1i("texture1", 0);
	m_Shader->SetUniform1i("texture2", 1);
	m_Shader->SetUniform1f("mixValue", m_MixValue);
	m_Shader->SetUniformsMat4f("transform", m_Trans);
	GLCall(glBindVertexArray(m_VAO));
	GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

	m_Trans = glm::mat4(1.0f); //���þ���
	m_Trans = glm::translate(m_Trans, glm::vec3(-0.5f, 0.5f, 0.0f));
	float scaleAmount = fabs(static_cast<float>(sin(glfwGetTime())));
	m_Trans = glm::scale(m_Trans, glm::vec3(scaleAmount, scaleAmount, scaleAmount));
	m_Shader->SetUniformsMat4f("transform", m_Trans);
	GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

	m_Trans = glm::mat4(1.0f); //���þ���
	m_Trans = glm::scale(m_Trans, glm::vec3(0.3f, 0.3f, 0.3f));
	m_Shader->SetUniformsMat4f("transform", m_Trans);
	GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

	GLCall(glBindVertexArray(0));
}

void test::TestMatrix::OnImGuiRender()
{
	ImGui::SliderFloat("MixValue", &m_MixValue, 0.0f, 1.0f);
	ImGui::SliderFloat("Rotate", &m_Rotate, -360.0f, 360.0f);
	ImGui::SliderFloat3("Scale", &m_Scale.x, 0.0f, 1.0f);
}