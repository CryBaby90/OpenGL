#include "TestShaderPart.h"
#include "GLCall.h"
#include "Shader.h"

#include <glm/glm.hpp>
#include <imgui/imgui.h>

test::TestShaderPart::TestShaderPart()
	:m_Shader(nullptr), m_Offset_x(0)
{
	//在上下文之后
	GLfloat vertices[] = {
		// 位置              // 颜色
	 0.5f, -0.5f, 0.0f,  1.0f, 0.0f, 0.0f,   // 右下
	-0.5f, -0.5f, 0.0f,  0.0f, 1.0f, 0.0f,   // 左下
	 0.0f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f    // 顶部
	};

	GLuint indices[] = {
		// 注意索引从0开始! 
		// 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
		// 这样可以由下标代表顶点组合成矩形
		0, 1, 3, // 第一个三角形
		1, 2, 3  // 第二个三角形
	};

	//  创建VAO, VBO, EBO
	GLCall(glGenVertexArrays(1, &m_VAO));
	GLCall(glGenBuffers(1, &m_VBO));//创建VBO
	GLCall(glGenBuffers(1, &m_EBO));//创建EBO = IBO:index buffer

	//1  绑定VAO缓冲对象
	GLCall(glBindVertexArray(m_VAO));

	//2  绑定VBO（顶点数组）到缓冲中供OpenGL使用
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));//顶点缓冲对象，顶点缓冲区 GL_ARRAY_BUFFER是一个缓冲目标
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));//填充数据
																					  //GL_STATIC_DRAW ：数据不会或几乎不会改变。
																					  //GL_DYNAMIC_DRAW：数据会被改变很多。
																					  //GL_STREAM_DRAW ：数据每次绘制时都会改变。

	//3  绑定EBO（索引数组）到一个索引缓冲中，供OpenGL使用 
	GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
	GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

	//4  设置顶点属性指针
	//顶点属性 layout(location = 0)
	//顶点属性的大小
	//参数指定数据的类型
	//是否希望数据被标准化(Normalize)。如果我们设置为GL_TRUE，所有数据都会被映射到0（对于有符号型signed数据是-1）到1之间。我们把它设置为GL_FALSE。
	//步长(Stride)
	//位置数据在缓冲中起始位置的偏移量(Offset)。由于位置数据在数组的开头，所以这里是0
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(0));

	GLCall(glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float))));
	GLCall(glEnableVertexAttribArray(1));

	//  解绑VAO  之前不要解绑VBO EBO 否则要重新绑定
	GLCall(glBindVertexArray(0));

	//6  着色器程序
	m_Shader = new Shader("res/shaders/ShaderPart/Vertex.Vshader", "res/shaders/ShaderPart/Fragment.Fshader");
}

test::TestShaderPart::~TestShaderPart()
{
	//资源释放
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
	//  绘制物体
	m_Shader->Bind(); //先glUseProgram
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
