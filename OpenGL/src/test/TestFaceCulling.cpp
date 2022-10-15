#include "TestFaceCulling.h"

#include <stb_image/stb_image.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>

test::TestFaceCulling::TestFaceCulling()
	:m_Shader(nullptr), m_Camera(nullptr),
	m_Model(glm::mat4(1.0f)), m_View(glm::mat4(1.0f)), m_Proj(glm::mat4(1.0f)),
	m_ViewPos(0.0f, 0.0f, -3.0f)
{
	//在上下文之后
	float cubeVertices[] = {
		// Back face
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
		 0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right         
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
		-0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		// Front face
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
		 0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
		-0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
		// Left face
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
		-0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
		-0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
		// Right face
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right         
		 0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
		 0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left     
		// Bottom face
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
		 0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
		 0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
		-0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
		-0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
		// Top face
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		 0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right     
		 0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
		-0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
		-0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left        
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
	//GLCall(glGenBuffers(1, &m_EBO));//创建EBO = IBO:index buffer

	//1  绑定VAO缓冲对象
	GLCall(glBindVertexArray(m_VAO));

	//2  绑定VBO（顶点数组）到缓冲中供OpenGL使用
	GLCall(glBindBuffer(GL_ARRAY_BUFFER, m_VBO));//顶点缓冲对象，顶点缓冲区 GL_ARRAY_BUFFER是一个缓冲目标
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


	//6  着色器程序
	m_Shader = new Shader("res/shaders/FaceCulling/Vertex.Vshader", "res/shaders/FaceCulling/Fragment.Fshader");

	//7	纹理
	
	//CubeTexture
	LoadImage(&m_CubeTextureID, "res/textures/container.jpg");
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

	m_Camera = std::make_unique<Camera>(glm::vec3(0.0f, 0.0f, 3.0f));

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

	//开启剔除
	GLCall(glEnable(GL_CULL_FACE));
	/*
	  GL_BACK：只剔除背向面。
	  GL_FRONT：只剔除正向面。
	  GL_FRONT_AND_BACK：剔除正向面和背向面。
	*/
	GLCall(glCullFace(GL_FRONT));

	/*
	* GL_CCW 逆时针
	* GL_CW  顺时针
	*/
	GLCall(glFrontFace(GL_CCW));
}

test::TestFaceCulling::~TestFaceCulling()
{
	//资源释放
	delete m_Shader;
	GLCall(glDeleteVertexArrays(1, &m_VAO));
	GLCall(glDeleteBuffers(1, &m_VBO));
	GLCall(glDeleteBuffers(1, &m_EBO));
}

void test::TestFaceCulling::OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset)
{
	m_Camera->OnProcessMouseMovement(xoffset, yoffset);
}

void test::TestFaceCulling::LoadImage(GLuint* textureID, char const* filename)
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

void test::TestFaceCulling::OnUpdate(float deltaTime)
{
	/*m_Model = glm::mat4(1.0f);
	m_Model = glm::rotate(m_Model, deltaTime * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 0.0f));

	m_View = glm::mat4(1.0f);
	m_View = glm::translate(m_View, m_ViewPos);*/
}

void test::TestFaceCulling::OnRender()
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

	//cube
	m_Model = glm::translate(m_Model, glm::vec3(-1.0f, 0.0f, -1.0f));
	m_Shader->SetUniformsMat4f("model", m_Model);
	GLCall(glBindVertexArray(m_VAO));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_CubeTextureID));
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36)); //没有设置index pos  暂时用glDrawArrays
	m_Model = glm::mat4(1.0f);
	m_Model = glm::translate(m_Model, glm::vec3(2.0f, 0.0f, 0.0f));
	m_Shader->SetUniformsMat4f("model", m_Model);
	GLCall(glDrawArrays(GL_TRIANGLES, 0, 36));

	GLCall(glBindVertexArray(0));
}

void test::TestFaceCulling::OnImGuiRender()
{
	//ImGui::SliderFloat3("ViewPos", &m_ViewPos.x, -6.0f, 6.0f);
}
