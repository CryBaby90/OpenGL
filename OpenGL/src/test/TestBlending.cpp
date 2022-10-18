#include "TestBlending.h"

#include <stb_image/stb_image.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>

test::TestBlending::TestBlending()
	:m_Shader(nullptr), m_SingleColorShader(nullptr), m_Camera(nullptr),
	m_Model(glm::mat4(1.0f)), m_View(glm::mat4(1.0f)), m_Proj(glm::mat4(1.0f)),
	m_ViewPos(0.0f, 0.0f, -3.0f)
{
	//在上下文之后
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
	float transparentVertices[] = {
		// positions         // texture Coords (swapped y coordinates because texture is flipped upside down)
		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		0.0f, -0.5f,  0.0f,  0.0f,  1.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,

		0.0f,  0.5f,  0.0f,  0.0f,  0.0f,
		1.0f, -0.5f,  0.0f,  1.0f,  1.0f,
		1.0f,  0.5f,  0.0f,  1.0f,  0.0f
	};

	//GLuint indices[] = {
	//	// 注意索引从0开始! 
	//	// 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
	//	// 这样可以由下标代表顶点组合成矩形
	//	0, 1, 3, // 第一个三角形
	//	1, 2, 3  // 第二个三角形
	//};

	//  创建VAO, VBO, EBO
	GLCall(glGenVertexArrays(1, &m_CubeVAO));
	GLCall(glGenBuffers(1, &m_CubeVBO));//创建VBO
	//GLCall(glGenBuffers(1, &m_EBO));//创建EBO = IBO:index buffer

	//1  绑定VAO缓冲对象
	GLCall(glBindVertexArray(m_CubeVAO));

	//2  绑定VBO（顶点数组）到缓冲中供OpenGL使用
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_CubeVBO));//顶点缓冲对象，顶点缓冲区 GL_ARRAY_BUFFER是一个缓冲目标
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), cubeVertices, GL_STATIC_DRAW));//填充数据
																					  //GL_STATIC_DRAW ：数据不会或几乎不会改变。
																					  //GL_DYNAMIC_DRAW：数据会被改变很多。
																					  //GL_STREAM_DRAW ：数据每次绘制时都会改变。

	//3  绑定EBO（索引数组）到一个索引缓冲中，供OpenGL使用 
	//GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO));
	//GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

	//4  设置顶点属性指针
	//顶点属性 layout(location = 0)
	//顶点属性的大小
	//参数指定数据的类型
	//是否希望数据被标准化(Normalize)。如果我们设置为GL_TRUE，所有数据都会被映射到0（对于有符号型signed数据是-1）到1之间。我们把它设置为GL_FALSE。
	//步长(Stride)
	//位置数据在缓冲中起始位置的偏移量(Offset)。由于位置数据在数组的开头，所以这里是0
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
	
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));

	//  解绑VAO  之前不要解绑VBO EBO 否则要重新绑定
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

	//transparen
	GLCall(glGenVertexArrays(1, &m_TransparenVAO));
	GLCall(glGenBuffers(1, &m_TransparenVBO));
	GLCall(glBindVertexArray(m_TransparenVAO));
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_TransparenVBO));
	GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(transparentVertices), transparentVertices, GL_STATIC_DRAW));
	GLCall(glEnableVertexAttribArray(0));
	GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0));
	GLCall(glEnableVertexAttribArray(1));
	GLCall(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))));
	GLCall(glBindVertexArray(0));

	//6  着色器程序
	m_Shader = new Shader("res/shaders/Blending/Vertex.Vshader", "res/shaders/Blending/Fragment.Fshader");
	m_SingleColorShader = new Shader("res/shaders/Blending/Vertex.Vshader", "res/shaders/Blending/ShaderSingleColor.Fshader");

	//7	纹理
	
	//CubeTexture
	LoadImage(&m_CubeTextureID, "res/textures/wood.png");
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
	

	//Plane
	LoadImage(&m_PlaneTextureID, "res/textures/brickDiffuse.jpg");
	// 为当前绑定的纹理对象设置环绕、过滤方式
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	//transparen
	//LoadImage(&m_TransparenTextureID, "res/textures/grass.png");
	LoadImage(&m_TransparenTextureID, "res/textures/blending_transparent_window.png");
	// 为当前绑定的纹理对象设置环绕、过滤方式
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR));
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));

	m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 6.0f));

	//开启z-buffer 见OpenGL 坐标系统
	GLCall(glEnable(GL_DEPTH_TEST));

	//深度测试函数
	/*
		GL_ALWAYS	永远通过深度测试
		GL_NEVER	永远不通过深度测试
		GL_LESS	在片段深度值小于缓冲的深度值时通过测试
		GL_EQUAL	在片段深度值等于缓冲区的深度值时通过测试
		GL_LEQUAL	在片段深度值小于等于缓冲区的深度值时通过测试
		GL_GREATER	在片段深度值大于缓冲区的深度值时通过测试
		GL_NOTEQUAL	在片段深度值不等于缓冲区的深度值时通过测试
		GL_GEQUAL	在片段深度值大于等于缓冲区的深度值时通过测试*/
	GLCall(glDepthFunc(GL_LESS)); //默认 less

	//不更新深度缓冲
	//GLCall(glDepthMask(GL_FALSE));

	//打开混合
	GLCall(glEnable(GL_BLEND));

	/*	GL_ZERO							因子等于0
		GL_ONE							因子等于1
		GL_SRC_COLOR					因子等于源颜色向量C¯source
		GL_ONE_MINUS_SRC_COLOR			因子等于1−C¯source
		GL_DST_COLOR					因子等于目标颜色向量C¯destination
		GL_ONE_MINUS_DST_COLOR			因子等于1−C¯destination
		GL_SRC_ALPHA					因子等于C¯source的alpha分量
		GL_ONE_MINUS_SRC_ALPHA			因子等于1− C¯source的alpha分量
		GL_DST_ALPHA					因子等于C¯destination的alpha分量
		GL_ONE_MINUS_DST_ALPHA			因子等于1− C¯destination的alpha分量
		GL_CONSTANT_COLOR				因子等于常数颜色向量C¯constant
		GL_ONE_MINUS_CONSTANT_COLOR		因子等于1−C¯constant
		GL_CONSTANT_ALPHA				因子等于C¯constant的alpha分量
		GL_ONE_MINUS_CONSTANT_ALPHA		因子等于1− C¯constant的alpha分量*/
	GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
	// 
	//为RGB和alpha通道分别设置不同的选项
	//GLCall(glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ZERO));

	/*
	* 设置运算符
	* GL_FUNC_ADD：默认选项，将两个分量相加：C¯result=Src+Dst。
	  GL_FUNC_SUBTRACT：将两个分量相减： C¯result=Src−Dst。
	  GL_FUNC_REVERSE_SUBTRACT：将两个分量相减，但顺序相反：C¯result=Dst−Src。
	*/
	//GLCall(glBlendEquation(GL_FUNC_ADD));

	/*
	先绘制所有不透明的物体。
	对所有透明的物体排序。
	按顺序绘制所有透明的物体。*/
}

test::TestBlending::~TestBlending()
{
	//资源释放
	delete m_Shader;
	GLCall(glDeleteVertexArrays(1, &m_CubeVAO));
	GLCall(glDeleteBuffers(1, &m_CubeVBO));
	GLCall(glDeleteVertexArrays(1, &m_PlaneVAO));
	GLCall(glDeleteBuffers(1, &m_PlaneVBO));
	GLCall(glDeleteVertexArrays(1, &m_TransparenVAO));
	GLCall(glDeleteBuffers(1, &m_TransparenVBO));
	//GLCall(glDeleteBuffers(1, &m_EBO));
}

void test::TestBlending::OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset)
{
	m_Camera->OnProcessMouseMovement(xoffset, yoffset);
}

void test::TestBlending::LoadImage(GLuint* textureID, char const* filename)
{
	GLCall(glGenTextures(1, textureID));
	GLCall(glBindTexture(GL_TEXTURE_2D, *textureID));
	//加载并生成纹理
	GLint width, height, nrChannels;
	stbi_set_flip_vertically_on_load(true);
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

		GLCall(glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, textureData));
		GLCall(glGenerateMipmap(GL_TEXTURE_2D));

		//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT)); // for this tutorial: use GL_CLAMP_TO_EDGE to prevent semi-transparent borders. Due to interpolation it takes texels from next repeat 
		//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, format == GL_RGBA ? GL_CLAMP_TO_EDGE : GL_REPEAT));
		//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR));
		//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR));
	}
	else
	{
		std::cout << "Failed to load texture1" << std::endl;
	}
	stbi_image_free(textureData);
}

void test::TestBlending::OnUpdate(float deltaTime)
{
	/*m_Model = glm::mat4(1.0f);
	m_Model = glm::rotate(m_Model, deltaTime * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

	m_View = glm::mat4(1.0f);
	m_View = glm::translate(m_View, m_ViewPos);*/
}

void test::TestBlending::OnRender()
{
	//每次渲染迭代前清除之前的深度缓存
	GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

	//  绘制物体
	//为图片设置插槽（纹理单元）
	GLCall(glActiveTexture(GL_TEXTURE0));

	m_Model = glm::mat4(1.0f);
	m_View = m_Camera->GetViewMatrix();
	m_Proj = m_Camera->GetProjMatrix();

	//先use shader
	m_Shader->Bind();
	//给shader里的变量指定插槽
	m_Shader->SetUniform1i("texture1", 0);
	m_Shader->SetUniformsMat4f("view", m_View);
	m_Shader->SetUniformsMat4f("proj", m_Proj);

	//plane
	GLCall(glBindVertexArray(m_PlaneVAO));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_PlaneTextureID));
	m_Shader->SetUniformsMat4f("model", m_Model);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
	GLCall(glBindVertexArray(0));

	//cube
	m_Model = glm::translate(m_Model, glm::vec3(-1.0f, 0.0f, -1.0f));
	m_Shader->SetUniformsMat4f("model", m_Model);
	GLCall(glBindVertexArray(m_CubeVAO));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_CubeTextureID));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36)); //没有设置index pos  暂时用glDrawArrays
	m_Model = glm::mat4(1.0f);
	m_Model = glm::translate(m_Model, glm::vec3(2.0f, 0.0f, 0.0f));
	m_Shader->SetUniformsMat4f("model", m_Model);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));

	// vegetation
	std::vector<glm::vec3> vegetation
	{
		glm::vec3(-1.5f, 0.0f, -0.48f),
		glm::vec3(1.5f, 0.0f, 0.51f),
		glm::vec3(0.0f, 0.0f, 0.7f),
		glm::vec3(-0.3f, 0.0f, -2.3f),
		glm::vec3(0.5f, 0.0f, -0.6f)
	};
	GLCall(glBindVertexArray(m_TransparenVAO));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_TransparenTextureID));
	/*for (unsigned int i = 0; i < vegetation.size(); i++)
	{
		m_Model = glm::mat4(1.0f);
		m_Model = glm::translate(m_Model, vegetation[i]);
		m_Shader->SetUniformsMat4f("model", m_Model);
		GLCall(glDrawArrays(GL_TRIANGLES, 0, 6));
	}*/

	//根据与摄像机的距离排序
	std::vector<glm::vec3> windows
	{
		glm::vec3(-1.5f, 0.0f, -0.48f),
		glm::vec3(1.5f, 0.0f, 0.51f),
		glm::vec3(0.0f, 0.0f, 0.7f),
		glm::vec3(-0.3f, 0.0f, -2.3f),
		glm::vec3(0.5f, 0.0f, -0.6f)
	};
	std::map<float, glm::vec3> sorted;
	for (unsigned int i = 0; i < windows.size(); i++)
	{
		float distance = glm::length(m_Camera->GetPos() - windows[i]);
		sorted[distance] = windows[i];//map会自动根据键值(Key)对它的值排序
	}
	for (std::map<float, glm::vec3>::reverse_iterator it = sorted.rbegin(); it != sorted.rend(); ++it)
	{
		//rbegin  rend 逆序
		m_Model = glm::mat4(1.0f);
		m_Model = glm::translate(m_Model, it->second);
		m_Shader->SetUniformsMat4f("model", m_Model);
		glDrawArrays(GL_TRIANGLES, 0, 6);
	}

	GLCall(glBindVertexArray(0));
}

void test::TestBlending::OnImGuiRender()
{
	//ImGui::SliderFloat3("ViewPos", &m_ViewPos.x, -6.0f, 6.0f);
}
