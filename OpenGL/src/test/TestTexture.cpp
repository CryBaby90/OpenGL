#include "TestTexture.h"
#include "GLCall.h"
#include "Shader.h"

#include <stb_image/stb_image.h>

test::TestTexture::TestTexture()
	:m_Shader(nullptr)
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
		// �����������±��������ϳɾ���
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
	m_Shader = new Shader("res/shaders/Texture/Vertex.Vshader", "res/shaders/Texture/Fragment.Fshader");

	//7	����
	GLuint textureID;
	GLCall(glGenTextures(1, &textureID));
	GLCall(glBindTexture(GL_TEXTURE_2D, textureID));
	// Ϊ��ǰ�󶨵�����������û��ơ����˷�ʽ
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	//���ز���������
	GLint width, height, nrChannels;
	unsigned char* textureData = stbi_load("res/textures/container.jpg", &width, &height, &nrChannels, 0);
	if (textureData)
	{
		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, textureData));
		GLCall(glGenerateMipmap(GL_TEXTURE_2D));
	}
	else
	{
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(textureData);
}

test::TestTexture::~TestTexture()
{
	//��Դ�ͷ�
	delete m_Shader;
	GLCall(glDeleteVertexArrays(1, &m_VAO));
	GLCall(glDeleteBuffers(1, &m_VBO));
	GLCall(glDeleteBuffers(1, &m_EBO));
}

void test::TestTexture::OnUpdate(float deltaTime)
{
}

void test::TestTexture::OnRender()
{
	//  ��������
	m_Shader->Bind();
	GLCall(glBindVertexArray(m_VAO));
	GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

	GLCall(glBindVertexArray(0));
}

void test::TestTexture::OnImGuiRender()
{
}
