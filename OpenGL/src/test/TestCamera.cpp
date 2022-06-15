#include "TestCamera.h"

#include <stb_image/stb_image.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

test::TestCamera::TestCamera()
	:m_Shader(nullptr), m_MixValue(0.2f), m_CameraSpeed(2.5f),
	m_Model(glm::mat4(1.0f)), m_View(glm::mat4(1.0f)), m_Proj(glm::mat4(1.0f)),
	m_CubePositions(nullptr), m_CameraPos(0.0f, 0.0f, 5.0f), m_CameraFront(0.0f, 0.0f, -1.0f), m_CameraUp(0.0f, 1.0f, 0.0f)
{
	//��������֮��
	GLfloat vertices[] = {
		//     ---- λ�� ----          - �������� -
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
	m_Shader = new Shader("res/shaders/Camera/Vertex.Vshader", "res/shaders/Camera/Fragment.Fshader");

	//7	����
	GLCall(glGenTextures(1, &m_TextureID1));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureID1));
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
	// Ϊ��ǰ�󶨵�����������û��ơ����˷�ʽ
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

	//ģ�;���
	m_Model = glm::rotate(m_Model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
	//�۲����
	// ע�⣬���ǽ�����������Ҫ�����ƶ������ķ������ƶ���
	m_View = glm::translate(m_View, glm::vec3(0.0f, 0.0f, -3.0f));
	//ͶӰ����
	m_Proj = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

	//����z-buffer
	GLCall(glEnable(GL_DEPTH_TEST));

	m_CubePositions = new glm::vec3[]{
		//glm::vec3(0.0f,  0.0f,  0.0f),
		glm::vec3(2.0f,  5.0f, -15.0f),
		glm::vec3(-1.5f, -2.2f, -2.5f),
		glm::vec3(-3.8f, -2.0f, -12.3f),
		glm::vec3(2.4f, -0.4f, -3.5f),
		glm::vec3(-1.7f,  3.0f, -7.5f),
		glm::vec3(1.3f, -2.0f, -2.5f),
		glm::vec3(1.5f,  2.0f, -2.5f),
		glm::vec3(1.5f,  0.2f, -1.5f),
		glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	//����������ռ�����
	//�����λ��
	glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
	//ԭ��
	glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f, 0.0f);
	//��������
	glm::vec3 cameraDirection = glm::normalize(cameraPos - cameraTarget);
	//����
	glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);
	glm::vec3 cameraRight = glm::normalize(glm::cross(up, cameraDirection));
	//����
	glm::vec3 cameraUp = glm::cross(cameraDirection, cameraRight);

	//����LookAt����
	glm::mat4 view(glm::mat4(1.0f));
	view = glm::lookAt(
		glm::vec3(0.0f, 0.0f, 0.3f),//λ��
		glm::vec3(0.0f, 0.0f, 0.0f),//Ŀ��
		glm::vec3(0.0f, 1.0f, 0.0f)//������
	);
}

test::TestCamera::~TestCamera()
{
	//��Դ�ͷ�
	delete m_Shader;
	delete m_CubePositions;
	GLCall(glDeleteVertexArrays(1, &m_VAO));
	GLCall(glDeleteBuffers(1, &m_VBO));
	GLCall(glDeleteBuffers(1, &m_EBO));
}

void test::TestCamera::OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch)
{
	Yaw += xoffset;
	Pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (Pitch > 89.0f)
			Pitch = 89.0f;
		if (Pitch < -89.0f)
			Pitch = -89.0f;
	}

	glm::vec3 front;
	//����direction
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	m_CameraFront = glm::normalize(front);
	glm::vec3 Right = glm::normalize(glm::cross(m_CameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	m_CameraUp = glm::normalize(glm::cross(Right, m_CameraFront));
}

void test::TestCamera::OnScroll(GLfloat xoffset, GLfloat yoffset)
{
	if (fov >= 1.0f && fov <= 45.0f)
		fov -= yoffset;
	if (fov <= 1.0f)
		fov = 1.0f;
	if (fov >= 45.0f)
		fov = 45.0f;

	m_Proj = glm::perspective(glm::radians(fov), 800.0f / 600.0f, 0.1f, 100.0f);
}

void test::TestCamera::OnProcessInput(GLFWwindow* window, GLfloat deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		m_CameraPos += m_CameraSpeed * m_CameraFront * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		m_CameraPos -= m_CameraSpeed * m_CameraFront * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		m_CameraPos -= glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * m_CameraSpeed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		m_CameraPos += glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * m_CameraSpeed * deltaTime;
}

void test::TestCamera::OnUpdate(float deltaTime)
{
	m_Model = glm::mat4(1.0f);
	m_Model = glm::rotate(m_Model, deltaTime * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

	/*m_View = glm::mat4(1.0f);
	float radius = 10.0f;
	float camX = sin(glfwGetTime()) * radius;
	float camZ = cos(glfwGetTime()) * radius;
	m_View = glm::lookAt(glm::vec3(camX, 0.0, camZ), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));*/

	m_View = glm::mat4(1.0f);
	m_View = glm::lookAt(m_CameraPos, m_CameraPos + m_CameraFront, m_CameraUp);
}

void test::TestCamera::OnRender()
{
	//ÿ����Ⱦ����ǰ���֮ǰ����Ȼ���
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));

	//  ��������
	//ΪͼƬ���ò�ۣ�����Ԫ��
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
	m_Shader->SetUniformsMat4f("model", m_Model);
	m_Shader->SetUniformsMat4f("view", m_View);
	m_Shader->SetUniformsMat4f("proj", m_Proj);
	GLCall(glBindVertexArray(m_VAO));
	//GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36)); //û������index pos  ��ʱ��glDrawArrays
	for (unsigned int i = 0; i < 10; i++)
	{
		glm::mat4 model(glm::mat4(1.0f));
		model = glm::translate(model, m_CubePositions[i]);
		float angle = 20.0f * i;
		model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
		model = glm::rotate(model, glm::radians((float)glfwGetTime() * 10), glm::vec3(1.0f, 0.3f, 0.5f));
		m_Shader->SetUniformsMat4f("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	GLCall(glBindVertexArray(0));
}

void test::TestCamera::OnImGuiRender()
{
	ImGui::SliderFloat("MixValue", &m_MixValue, 0.0f, 1.0f);
	ImGui::SliderFloat3("CameraPos", &m_CameraPos.x, -30, 30);
	ImGui::InputFloat("CameraSpeed", &m_CameraSpeed);
}
