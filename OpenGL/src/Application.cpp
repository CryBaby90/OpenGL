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


	//����window
	GLFWwindow* window = glfwCreateWindow(800, 600, "MyOpenGL", NULL, NULL);
	if (!window)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
	//����������
	glfwMakeContextCurrent(window);

	//���ü����֡����
	glfwSwapInterval(1);

	//glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}

	//������Ⱦ����
	GLCall(glViewport(0, 0, 800, 600));
	//��ק�ص� �ı���Ⱦ���ڴ�С
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	//�߿�ģʽ
	//GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
	//Ĭ��ģʽ
	GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

	// ���� ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

	// ���� ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// ���� ImGui Platform/Renderer backends
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

	//��Ⱦѭ��
	while (!glfwWindowShouldClose(window))
	{
		//����
		processInput(window);

		//��Ⱦָ��
		GLCall(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));//��������  GL_COLOR_BUFFER_BIT������ɫ����

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

		// ��鲢�����¼�����������
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

	//�ر�
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}