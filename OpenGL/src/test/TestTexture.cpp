#include "TestTexture.h"
#include "GLCall.h"
#include "Shader.h"

#include <stb_image/stb_image.h>

test::TestTexture::TestTexture()
	:m_Shader(nullptr)
{
	//在上下文之后
	GLfloat vertices[] = {
		//     ---- 位置 ----          - 纹理坐标 -
	 0.5f,  0.5f, 0.0f,					1.0f, 1.0f,   // 右上
	 0.5f, -0.5f, 0.0f,				    1.0f, 0.0f,   // 右下
	-0.5f, -0.5f, 0.0f,					0.0f, 0.0f,   // 左下
	-0.5f,  0.5f, 0.0f,					0.0f, 1.0f    // 左上
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
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(0));

	GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));
	GLCall(glEnableVertexAttribArray(1));

	//  解绑VAO  之前不要解绑VBO EBO 否则要重新绑定
	GLCall(glBindVertexArray(0));

	//6  着色器程序
	m_Shader = new Shader("res/shaders/Texture/Vertex.Vshader", "res/shaders/Texture/Fragment.Fshader");

	//7	纹理
	GLuint textureID;
	GLCall(glGenTextures(1, &textureID));
	GLCall(glBindTexture(GL_TEXTURE_2D, textureID));
	// 为当前绑定的纹理对象设置环绕、过滤方式
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	//加载并生成纹理
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
	//资源释放
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
	//  绘制物体
	m_Shader->Bind();
	GLCall(glBindVertexArray(m_VAO));
	GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

	GLCall(glBindVertexArray(0));
}

void test::TestTexture::OnImGuiRender()
{
}
