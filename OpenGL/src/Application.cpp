#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Application.h"
#include "test/TestClearColor.h"
#include "test/TestStandDraw.h"
#include "test/TestShaderPart.h"
#include "test/TestTexture.h"
#include "test/TestMatrix.h"
#include "test/TestCoordinate.h"

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

	//线框模式
	//GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
	//默认模式
	GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

	// 设置 ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// 设置 ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// 设置 ImGui Platform/Renderer backends
	const char* glsl_version = "#version 330";
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	::test::Test* currentTest = nullptr;
	::test::TestMenu* testMenu = new test::TestMenu(currentTest);
	currentTest = testMenu;
	testMenu->RegisterTest<::test::TestClearColor>("Clear Color");
	testMenu->RegisterTest<::test::TestStandDraw>("Stand Draw");
	testMenu->RegisterTest<::test::TestShaderPart>("Shader");
	testMenu->RegisterTest<::test::TestTexture>("Texture");
	testMenu->RegisterTest<::test::TestMatrix>("Matrix");
	testMenu->RegisterTest<::test::TestCoordinate>("TestCoordinate");

	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

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

		//ImGui
		if (currentTest)
		{
			currentTest->OnUpdate((float)glfwGetTime());
			currentTest->OnRender();
			ImGui::Begin("Test");
			if (currentTest != testMenu && ImGui::Button("<-"))
			{
				delete currentTest;
				currentTest = testMenu;
			}
			currentTest->OnImGuiRender();
			ImGui::End();
		}

		// ImGui Rendering  
		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		// 检查并调用事件，交换缓冲
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	delete currentTest;
	if (currentTest != testMenu)
		delete testMenu;

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	//关闭
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}