#include "TestMatrix.h"
#include "GLCall.h"
#include "Shader.h"

#include <stb_image/stb_image.h>
#include <imgui/imgui.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

test::TestMatrix::TestMatrix()
	:m_Shader(nullptr), m_MixValue(0.2f), m_Trans(glm::mat4(1.0f)), 
	m_Rotate(0.0f), m_Scale(1.0f, 1.0f, 1.0f)
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
	m_Shader = new Shader("res/shaders/Matrix/Vertex.Vshader", "res/shaders/Matrix/Fragment.Fshader");

	//7	纹理
	GLCall(glGenTextures(1, &m_TextureID1));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureID1));
	// 为当前绑定的纹理对象设置环绕、过滤方式
	//GL_REPEAT	对纹理的默认行为。重复纹理图像。
	//GL_MIRRORED_REPEAT	和GL_REPEAT一样，但每次重复图片是镜像放置的
	//GL_CLAMP_TO_EDGE	纹理坐标会被约束在0到1之间，超出的部分会重复纹理坐标的边缘，产生一种边缘被拉伸的效果。
	//GL_CLAMP_TO_BORDER	超出的坐标为用户指定的边缘颜色。
	//如果用GL_CLAMP_TO_BORDER 需要指定边缘颜色：float borderColor[] = { 1.0f, 1.0f, 0.0f, 1.0f };
	//											glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	//GL_NEAREST				邻近过滤 会选择中心点最接近纹理坐标的那个像素
	//GL_LINEAR					线性过滤 它会基于纹理坐标附近的纹理像素，计算出一个插值，近似出这些纹理像素之间的颜色
	//GL_NEAREST_MIPMAP_NEAREST	使用最邻近的多级渐远纹理来匹配像素大小，并使用邻近插值进行纹理采样
	//GL_LINEAR_MIPMAP_NEAREST	使用最邻近的多级渐远纹理级别，并使用线性插值进行采样
	//GL_NEAREST_MIPMAP_LINEAR	在两个最匹配像素大小的多级渐远纹理之间进行线性插值，使用邻近插值进行采样
	//GL_LINEAR_MIPMAP_LINEAR	在两个邻近的多级渐远纹理之间使用线性插值，并使用线性插值进行采样
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	//加载并生成纹理
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
	// 为当前绑定的纹理对象设置环绕、过滤方式
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
}

test::TestMatrix::~TestMatrix()
{
	//资源释放
	delete m_Shader;
	GLCall(glDeleteVertexArrays(1, &m_VAO));
	GLCall(glDeleteBuffers(1, &m_VBO));
	GLCall(glDeleteBuffers(1, &m_EBO));
}

void test::TestMatrix::OnUpdate(float deltaTime)
{
	m_Trans = glm::rotate(m_Trans, glm::radians(m_Rotate), glm::vec3(0.0, 0.0, 1.0));
	m_Trans = glm::scale(m_Trans, m_Scale);
}

void test::TestMatrix::OnRender()
{
	//  绘制物体
	//为图片设置插槽（纹理单元）
	GLCall(glActiveTexture(GL_TEXTURE0));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureID1));
	GLCall(glActiveTexture(GL_TEXTURE1));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TextureID2));
	//先use shader
	m_Shader->Bind();
	//给shader里的变量指定插槽
	m_Shader->SetUniform1i("texture1", 0);
	m_Shader->SetUniform1i("texture2", 1);
	m_Shader->SetUniform1f("mixValue", m_MixValue);
	m_Shader->SetUniformsMat4f("transform", m_Trans);
	GLCall(glBindVertexArray(m_VAO));
	GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

	GLCall(glBindVertexArray(0));
}

void test::TestMatrix::OnImGuiRender()
{
	ImGui::SliderFloat("MixValue", &m_MixValue, 0.0f, 1.0f);
	ImGui::SliderFloat("Rotate", &m_Rotate, -360.0f, 360.0f);
	ImGui::SliderFloat3("Scale", &m_Scale.x, 0.0f, 1.0f);
}
