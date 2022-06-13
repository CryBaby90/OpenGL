#include "TestShaderPart.h"
#include "GLCall.h"
#include "Shader.h"

#include <glm/glm.hpp>
#include <imgui/imgui.h>

test::TestShaderPart::TestShaderPart()
	:m_Shader(nullptr), m_Offset_x(0)
{
	//��������֮��
	GLfloat vertices[] = {
		// λ��              // ��ɫ
	 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // ����
	-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // ����
	 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // ����
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
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(0));

	GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))));
	GLCall(glEnableVertexAttribArray(1));

	//  ���VAO  ֮ǰ��Ҫ���VBO EBO ����Ҫ���°�
	GLCall(glBindVertexArray(0));

	//6  ��ɫ������
	m_Shader = new Shader("res/shaders/ShaderPart/Vertex.Vshader", "res/shaders/ShaderPart/Fragment.Fshader");
}

test::TestShaderPart::~TestShaderPart()
{
	//��Դ�ͷ�
	delete m_Shader;
	GLCall(glDeleteVertexArrays(1, &m_VAO));
	GLCall(glDeleteBuffers(1, &m_VBO));
	GLCall(glDeleteBuffers(1, &m_EBO));
}

void test::TestShaderPart::OnUpdate(float deltaTime)
{
}

void test::TestShaderPart::OnRender()
{
	//  ��������
	m_Shader->Bind(); //��glUseProgram
	float timeValue = glfwGetTime();
	float greenValue = (sin(timeValue) / 2.0f) + 0.5f;
	m_Shader->SetUniform1f("offset_x", m_Offset_x);
	m_Shader->SetUniforms4f("ourColor", ::glm::vec4(0.0f, greenValue, 0.0f, 1.0f));
	GLCall(glBindVertexArray(m_VAO));
	GLCall(glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0));

	GLCall(glBindVertexArray(0));
}

void test::TestShaderPart::OnImGuiRender()
{
	ImGui::SliderFloat("Translation A", &m_Offset_x, -0.5f, 0.5f);
}
