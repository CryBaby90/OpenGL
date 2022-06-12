#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Application.h"
#include "Shader.h"

int main()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif


	//创建window
	GLFWwindow* window = glfwCreateWindow(800, 600, "MyOpenGL", NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	//创建上下文
	glfwMakeContextCurrent(window);

	//设置间隔（帧数）
	glfwSwapInterval(1);

	//glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//创建渲染窗口
	GLCall(glViewport(0, 0, 800, 600));
	//拖拽回调 改变渲染窗口大小
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// 设置 ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// 设置 ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// 设置 ImGui Platform/Renderer backends
	const char* glsl_version = "#version 130";
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	{
		//在上下文之后
		GLfloat vertices[] = {
			0.5f, 0.5f, 0.0f,   // 右上角
			0.5f, -0.5f, 0.0f,  // 右下角
			-0.5f, -0.5f, 0.0f, // 左下角
			-0.5f, 0.5f, 0.0f   // 左上角
		};

		GLuint indices[] = {
			// 注意索引从0开始! 
			// 此例的索引(0,1,2,3)就是顶点数组vertices的下标，
			// 这样可以由下标代表顶点组合成矩形
			0, 1, 3, // 第一个三角形
			1, 2, 3  // 第二个三角形
		};

		//  创建VAO, VBO, EBO
		GLuint VAO, VBO, EBO;
		GLCall(glGenVertexArrays(1, &VAO));
		GLCall(glGenBuffers(1, &VBO));//创建VBO
		GLCall(glGenBuffers(1, &EBO));//创建EBO = IBO:index buffer

		//1  绑定VAO缓冲对象
		GLCall(glBindVertexArray(VAO));

		//2  绑定VBO（顶点数组）到缓冲中供OpenGL使用
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));//顶点缓冲对象，顶点缓冲区 GL_ARRAY_BUFFER是一个缓冲目标
		GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));//填充数据
																						  //GL_STATIC_DRAW ：数据不会或几乎不会改变。
																						  //GL_DYNAMIC_DRAW：数据会被改变很多。
																						  //GL_STREAM_DRAW ：数据每次绘制时都会改变。

		//3  绑定EBO（索引数组）到一个索引缓冲中，供OpenGL使用 
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

		//4  设置顶点属性指针
		//顶点属性 layout(location = 0)
		//顶点属性的大小
		//参数指定数据的类型
		//是否希望数据被标准化(Normalize)。如果我们设置为GL_TRUE，所有数据都会被映射到0（对于有符号型signed数据是-1）到1之间。我们把它设置为GL_FALSE。
		//步长(Stride)
		//位置数据在缓冲中起始位置的偏移量(Offset)。由于位置数据在数组的开头，所以这里是0
		GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
		GLCall(glEnableVertexAttribArray(0));

		//  解绑VAO  之前不要解绑VBO EBO 否则要重新绑定
		GLCall(glBindVertexArray(0));


		//6  着色器程序
		Shader shader("res/shaders/Vertex.Vshader", "res/shaders/Fragment.Fshader");
		
		//线框模式
		//GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
		//默认模式
		GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

		//渲染循环
		while (!glfwWindowShouldClose(window))
		{
			//输入
			processInput(window);

			//渲染指令
			GLCall(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
			GLCall(glClear(GL_COLOR_BUFFER_BIT));//清屏函数  GL_COLOR_BUFFER_BIT清理颜色缓冲

			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			{//ImGui
				static float f = 0.0f;
				static int counter = 0;

				ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

				ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)

				if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
					counter++;
				ImGui::SameLine();
				ImGui::Text("counter = %d", counter);

				ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
				ImGui::End();
			}

			//  绘制物体
			shader.Bind();
			GLCall(glBindVertexArray(VAO));
			GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

			GLCall(glBindVertexArray(0));

			// ImGui Rendering  
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			// 检查并调用事件，交换缓冲
			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		//资源释放
		GLCall(glDeleteVertexArrays(1, &VAO));
		GLCall(glDeleteBuffers(1, &VBO));
		GLCall(glDeleteBuffers(1, &EBO));
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	//关闭
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}