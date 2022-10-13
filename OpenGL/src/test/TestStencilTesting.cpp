#include "TestStencilTesting.h"

#include <stb_image/stb_image.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

test::TestStencilTesting::TestStencilTesting()
	:m_Shader(nullptr), m_SingleColorShader(nullptr), m_Camera(nullptr),
	m_Model(glm::mat4(1.0f)), m_View(glm::mat4(1.0f)), m_Proj(glm::mat4(1.0f)),
	m_ViewPos(0.0f, 0.0f, -3.0f)
{
	//��������֮��
	float cubeVertices[] = {
		// positions          // texture Coords
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	float planeVertices[] = {
		// positions          // texture Coords (note we set these higher than 1 (together with GL_REPEAT as texture wrapping mode). this will cause the floor texture to repeat)
		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f,  5.0f,  0.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,

		 5.0f, -0.5f,  5.0f,  2.0f, 0.0f,
		-5.0f, -0.5f, -5.0f,  0.0f, 2.0f,
		 5.0f, -0.5f, -5.0f,  2.0f, 2.0f
	};

	//GLuint indices[] = {
	//	// ע��������0��ʼ! 
	//	// ����������(0,1,2,3)���Ƕ�������vertices���±꣬
	//	// �����������±��������ϳɾ���
	//	0, 1, 3, // ��һ��������
	//	1, 2, 3  // �ڶ���������
	//};

	//  ����VAO, VBO, EBO
	GLCall(glGenVertexArrays(1, &m_CubeVAO));
	GLCall(glGenBuffers(1, &m_CubeVBO));//����VBO
	//GLCall(glGenBuffers(1, &m_EBO));//����EBO = IBO:index buffer

	//1  ��VAO�������
	GLCall(glBindVertexArray(m_CubeVAO));

	//2  ��VBO���������飩�������й�OpenGLʹ��
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_CubeVBO));//���㻺����󣬶��㻺���� GL_ARRAY_BUFFER��һ������Ŀ��
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW));//�������
																					  //GL_STATIC_DRAW �����ݲ���򼸺�����ı䡣
																					  //GL_DYNAMIC_DRAW�����ݻᱻ�ı�ܶࡣ
																					  //GL_STREAM_DRAW ������ÿ�λ���ʱ����ı䡣

	//3  ��EBO���������飩��һ�����������У���OpenGLʹ�� 
	//GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
	//GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

	//4  ���ö�������ָ��
	//�������� layout(location = 0)
	//�������ԵĴ�С
	//����ָ�����ݵ�����
	//�Ƿ�ϣ�����ݱ���׼��(Normalize)�������������ΪGL_TRUE���������ݶ��ᱻӳ�䵽0�������з�����signed������-1����1֮�䡣���ǰ�������ΪGL_FALSE��
	//����(Stride)
	//λ�������ڻ�������ʼλ�õ�ƫ����(Offset)������λ������������Ŀ�ͷ������������0
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
	
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));

	//  ���VAO  ֮ǰ��Ҫ���VBO EBO ����Ҫ���°�
	GLCall(glBindVertexArray(0));

	// Plane
	GLCall(glGenVertexArrays(1, &m_PlaneVAO));
	GLCall(glGenBuffers(1, &m_PlaneVBO));
	GLCall(glBindVertexArray(m_PlaneVAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_PlaneVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));
	GLCall(glBindVertexArray(0));

	//6  ��ɫ������
	m_Shader = new Shader("res/shaders/StencilTesting/Vertex.Vshader", "res/shaders/StencilTesting/Fragment.Fshader");
	m_SingleColorShader = new Shader("res/shaders/StencilTesting/Vertex.Vshader", "res/shaders/StencilTesting/ShaderSingleColor.Fshader");

	//7	����
	GLCall(glGenTextures(1, &m_CubeTextureID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_CubeTextureID));
	// Ϊ��ǰ�󶨵�����������û��ơ����˷�ʽ
	//GL_REPEAT	�������Ĭ����Ϊ���ظ�����ͼ��
	//GL_MIRRORED_REPEAT	��GL_REPEATһ������ÿ���ظ�ͼƬ�Ǿ�����õ�
	//GL_CLAMP_TO_EDGE	��������ᱻԼ����0��1֮�䣬�����Ĳ��ֻ��ظ���������ı�Ե������һ�ֱ�Ե�������Ч����
	//GL_CLAMP_TO_BORDER	����������Ϊ�û�ָ���ı�Ե��ɫ��
	//�����GL_CLAMP_TO_BORDER ��Ҫָ����Ե��ɫ��float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	//											glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	//GL_NEAREST				�ڽ����� ��ѡ�����ĵ���ӽ�����������Ǹ�����
	//GL_LINEAR					���Թ��� ��������������긽�����������أ������һ����ֵ�����Ƴ���Щ��������֮�����ɫ
	//GL_NEAREST_MIPMAP_NEAREST	ʹ�����ڽ��Ķ༶��Զ������ƥ�����ش�С����ʹ���ڽ���ֵ�����������
	//GL_LINEAR_MIPMAP_NEAREST	ʹ�����ڽ��Ķ༶��Զ�����𣬲�ʹ�����Բ�ֵ���в���
	//GL_NEAREST_MIPMAP_LINEAR	��������ƥ�����ش�С�Ķ༶��Զ����֮��������Բ�ֵ��ʹ���ڽ���ֵ���в���
	//GL_LINEAR_MIPMAP_LINEAR	�������ڽ��Ķ༶��Զ����֮��ʹ�����Բ�ֵ����ʹ�����Բ�ֵ���в���
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	//���ز���������
	GLint width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
	unsigned char* textureData = stbi_load("res/textures/wood.png", &width, &height, &nrChannels, 0);
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

	GLCall(glGenTextures(1, &m_PlaneTextureID));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_PlaneTextureID));
	// Ϊ��ǰ�󶨵�����������û��ơ����˷�ʽ
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	textureData = stbi_load("res/textures/brickDiffuse.jpg", &width, &height, &nrChannels, 0);
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

	m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 6.0f));

	//����z-buffer ��OpenGL ����ϵͳ
	GLCall(glEnable(GL_DEPTH_TEST));

	//��Ȳ��Ժ���
	/*
		GL_ALWAYS	��Զͨ����Ȳ���
		GL_NEVER	��Զ��ͨ����Ȳ���
		GL_LESS	��Ƭ�����ֵС�ڻ�������ֵʱͨ������
		GL_EQUAL	��Ƭ�����ֵ���ڻ����������ֵʱͨ������
		GL_LEQUAL	��Ƭ�����ֵС�ڵ��ڻ����������ֵʱͨ������
		GL_GREATER	��Ƭ�����ֵ���ڻ����������ֵʱͨ������
		GL_NOTEQUAL	��Ƭ�����ֵ�����ڻ����������ֵʱͨ������
		GL_GEQUAL	��Ƭ�����ֵ���ڵ��ڻ����������ֵʱͨ������*/
	GLCall(glDepthFunc(GL_LESS)); //Ĭ�� less

	//��������Ȼ���
	//GLCall(glDepthMask(GL_FALSE));

	// ����ģ�����
	GLCall(glEnable(GL_STENCIL_TEST));

	/// <summary>
	/// ģ����Ժ���  �ο�ֵ  mask
	/// ���Ժ���ͬ��Ȳ��� 
	/// ����OpenGL ��ģ�建����ʲô
	/// </summary>
	GLCall(glStencilFunc(GL_EQUAL, 1, 0xFF));

	/// <summary>
	/// ģ�����ʧ��ʱ��ȡ����Ϊ  
	/// ģ�����ͨ�� ��Ȳ���ʧ�ܲ�ȡ��Ϊ
	/// ģ����Ժ���Ȳ��Զ�ͨ����ȡ����Ϊ
	/// ���»���
	/// </summary>
	/*	GL_KEEP	���ֵ�ǰ�����ģ��ֵ
		GL_ZERO	��ģ��ֵ����Ϊ0
		GL_REPLACE	��ģ��ֵ����ΪglStencilFunc�������õ�refֵ
		GL_INCR	���ģ��ֵС�����ֵ��ģ��ֵ��1
		GL_INCR_WRAP	��GL_INCRһ���������ģ��ֵ���������ֵ�����
		GL_DECR	���ģ��ֵ������Сֵ��ģ��ֵ��1
		GL_DECR_WRAP	��GL_DECRһ���������ģ��ֵС��0��������Ϊ���ֵ
		GL_INVERT	��λ��ת��ǰ��ģ�建��ֵ*/
	GLCall(glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE));

	/// <summary>
	/// ��������ֵ  
	/// </summary>
	//GLCall(glStencilMask(0xFF)); // ÿһλд��ģ�建��ʱ������ԭ��
	//GLCall(glStencilMask(0x00)); // ÿһλ��д��ģ�建��ʱ������0������д�룩
}

test::TestStencilTesting::~TestStencilTesting()
{
	//��Դ�ͷ�
	delete m_Shader;
	GLCall(glDeleteVertexArrays(1, &m_CubeVAO));
	GLCall(glDeleteBuffers(1, &m_CubeVBO));
	GLCall(glDeleteVertexArrays(1, &m_PlaneVAO));
	GLCall(glDeleteBuffers(1, &m_PlaneVBO));
	//GLCall(glDeleteBuffers(1, &m_EBO));
}

void test::TestStencilTesting::OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset)
{
	m_Camera->OnProcessMouseMovement(xoffset, yoffset);
}

void test::TestStencilTesting::OnUpdate(float deltaTime)
{
	/*m_Model = glm::mat4(1.0f);
	m_Model = glm::rotate(m_Model, deltaTime * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

	m_View = glm::mat4(1.0f);
	m_View = glm::translate(m_View, m_ViewPos);*/
}

void test::TestStencilTesting::OnRender()
{
	//ÿ����Ⱦ����ǰ���֮ǰ����Ȼ���
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

	//  ��������
	//ΪͼƬ���ò�ۣ�����Ԫ��
	GLCall(glActiveTexture(GL_TEXTURE0));

	m_Model = glm::mat4(1.0f);
	m_View = m_Camera->GetViewMatrix();
	m_Proj = m_Camera->GetProjMatrix();

	//��use shader
	m_Shader->Bind();
	//��shader��ı���ָ�����
	m_Shader->SetUniform1i("texture1", 0);
	m_Shader->SetUniformsMat4f("view", m_View);
	m_Shader->SetUniformsMat4f("proj", m_Proj);

	//plane
	GLCall(glStencilMask(0x00)); //ÿһλ��д��ģ�建��ʱ������0������д�룩

	GLCall(glBindVertexArray(m_PlaneVAO));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_PlaneTextureID));
	m_Shader->SetUniformsMat4f("model", m_Model);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
	GLCall(glBindVertexArray(0));

	//cube
	GLCall(glStencilFunc(GL_ALWAYS, 1, 0xFF));
	GLCall(glStencilMask(0xFF)); //ÿһλд��ģ�建��ʱ������ԭ��

	m_Model = glm::translate(m_Model, glm::vec3(-1.0f, 0.0f, -1.0f));
	m_Shader->SetUniformsMat4f("model", m_Model);
	GLCall(glBindVertexArray(m_CubeVAO));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_CubeTextureID));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36)); //û������index pos  ��ʱ��glDrawArrays
	m_Model = glm::mat4(1.0f);
	m_Model = glm::translate(m_Model, glm::vec3(2.0f, 0.0f, 0.0f));
	m_Shader->SetUniformsMat4f("model", m_Model);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));

	GLCall(glStencilFunc(GL_NOTEQUAL, 1, 0xFF));
	GLCall(glStencilMask(0x00));
	GLCall(glDisable(GL_DEPTH_TEST));
	float scale = 1.1f;
	m_SingleColorShader->Bind();
	m_SingleColorShader->SetUniformsMat4f("view", m_View);
	m_SingleColorShader->SetUniformsMat4f("proj", m_Proj);
	m_Model = glm::mat4(1.0f);
	m_Model = glm::translate(m_Model, glm::vec3(-1.0f, 0.0f, -1.0f));
	m_Model = glm::scale(m_Model, glm::vec3(scale, scale, scale));
	m_SingleColorShader->SetUniformsMat4f("model", m_Model);
	glDrawArrays(GL_TRIANGLES, 0, 36);
	m_Model = glm::mat4(1.0f);
	m_Model = glm::translate(m_Model, glm::vec3(2.0f, 0.0f, 0.0f));
	m_Model = glm::scale(m_Model, glm::vec3(scale, scale, scale));
	m_Shader->SetUniformsMat4f("model", m_Model);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));

	GLCall(glBindVertexArray(0));
	GLCall(glStencilMask(0xFF));
	GLCall(glStencilFunc(GL_ALWAYS, 0, 0xFF));
	GLCall(glEnable(GL_DEPTH_TEST));
}

void test::TestStencilTesting::OnImGuiRender()
{
	//ImGui::SliderFloat3("ViewPos", &m_ViewPos.x, -6.0f, 6.0f);
}
