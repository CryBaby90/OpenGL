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
#include "test/TestCamera.h"
#include "test/TestLightColor.h"
#include "test/TestBaseLight.h"
#include "test/TestMaterialLight.h"
#include "test/TestDiffuseMap.h"
#include "test/TestLightCasters.h"
#include "test/TestMultipleLights.h"
#include "test/TestModel.h"
#include "test/TestDepthTesting.h"
#include "test/TestStencilTesting.h"
#include "test/TestBlending.h"
#include "test/TestFaceCulling.h"
#include "test/TestFramebuffers.h"
#include "test/TestCubemaps.h"
#include "test/TestAdvancedData.h"
#include "test/TestAdvancedGLSL.h"
#include "test/TestGeometryShader.h"
#include "test/TestInstancing.h"
#include "test/TestPlanetInstancing.h"
#include "test/TestAntiAliasing.h"
#include "test/TestAdvancedLighting.h"
#include "test/TestGammaCorrection.h"
#include "test/TestShadowMapping.h"
#include "test/TestPointShadows.h"
#include "test/TestNormalMapping.h"
#include "test/TestParallaxMapping.h"
#include "test/TestHDR.h"
#include "test/TestBloom.h"
#include "test/TestDeferredShading.h"
#include "test/TestSSAO.h"
#include "test/PBRPipleline/PBRLighting/TesPBRLighting.h"

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
	GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "MyOpenGL", NULL, NULL);
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
	GLCall(glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT));
	//拖拽回调 改变渲染窗口大小
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	//设置鼠标光标
	if (showCursor)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

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

	GLCall(glfwSetCursorPosCallback(window, mouse_callback)); //因为回调调用了 ImGUI
	GLCall(glfwSetScrollCallback(window, scroll_callback));

	currentTest = testMenu;
	testMenu->RegisterTest<::test::TestClearColor>("Clear Color");
	testMenu->RegisterTest<::test::TestStandDraw>("Stand Draw");
	testMenu->RegisterTest<::test::TestShaderPart>("Shader");
	testMenu->RegisterTest<::test::TestTexture>("Texture");
	testMenu->RegisterTest<::test::TestMatrix>("Matrix");
	testMenu->RegisterTest<::test::TestCoordinate>("Coordinate");
	testMenu->RegisterTest<::test::TestCamera>("Camera");
	testMenu->RegisterTest<::test::TestLightColor>("LightColor");
	testMenu->RegisterTest<::test::TestBaseLight>("BaseLight");
	testMenu->RegisterTest<::test::TestMaterialLight>("MaterialLight");
	testMenu->RegisterTest<::test::TestDiffuseMap>("DiffuseMap");
	testMenu->RegisterTest<::test::TestLightCasters>("LightCasters");
	testMenu->RegisterTest<::test::TestMultipleLights>("MultipleLights");
	testMenu->RegisterTest<::test::TestModel>("Model");
	testMenu->RegisterTest<::test::TestDepthTesting>("DepthTesting");
	testMenu->RegisterTest<::test::TestStencilTesting>("StencilTesting");
	testMenu->RegisterTest<::test::TestBlending>("Blending");
	testMenu->RegisterTest<::test::TestFaceCulling>("FaceCulling");
	testMenu->RegisterTest<::test::TestFramebuffers>("Framebuffers");
	testMenu->RegisterTest<::test::TestCubemaps>("Cubemaps");
	testMenu->RegisterTest<::test::TestAdvancedData>("AdvancedData");
	testMenu->RegisterTest<::test::TestAdvancedGLSL>("AdvancedGLSL");
	testMenu->RegisterTest<::test::TestGeometryShader>("GeometryShader");
	testMenu->RegisterTest<::test::TestInstancing>("Instancing");
	testMenu->RegisterTest<::test::TestPlanetInstancing>("PlanetInstancing");
	testMenu->RegisterTest<::test::TestAntiAliasing>("AntiAliasing");
	testMenu->RegisterTest<::test::TestAdvancedLighting>("AdvancedLighting");
	testMenu->RegisterTest<::test::TestGammaCorrection>("GammaCorrection");
	testMenu->RegisterTest<::test::TestShadowMapping>("ShadowMapping");
	testMenu->RegisterTest<::test::TestPointShadows>("PointShadows");
	testMenu->RegisterTest<::test::TestNormalMapping>("NormalMapping");
	testMenu->RegisterTest<::test::TestParallaxMapping>("ParallaxMapping");
	testMenu->RegisterTest<::test::TestHDR>("HDR");
	testMenu->RegisterTest<::test::TestBloom>("Bloom");
	testMenu->RegisterTest<::test::TestDeferredShading>("DeferredShading");
	testMenu->RegisterTest<::test::TestSSAO>("SSAO");
	testMenu->RegisterTest<::test::TesPBRLighting>("PBRLighting");

	int nrAttributes;
	glGetIntegerv(GL_MAX_VERTEX_ATTRIBS, &nrAttributes);
	std::cout << "Maximum nr of vertex attributes supported: " << nrAttributes << std::endl;

	GLfloat deltaTime = 0.0f; // 当前帧与上一帧的时间差
	GLfloat lastFrame = 0.0f; // 上一帧的时间

	//渲染循环
	while (!glfwWindowShouldClose(window))
	{
		GLfloat currentFrame = static_cast<GLfloat>(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

		//输入
		processInput(window);

		//渲染指令
		//GLCall(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
		GLCall(glClearColor(0.1f, 0.1f, 0.1f, 1.0f));
		/*
		* 清屏函数  GL_COLOR_BUFFER_BIT清理颜色缓冲
		* 清屏函数  GL_DEPTH_BUFFER_BIT清理深度缓冲
		* 清屏函数  GL_STENCIL_BUFFER_BIT清理模板缓冲
		*/
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT));
																   
		// Start the Dear ImGui frame
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		//ImGui
		if (currentTest)
		{
			currentTest->OnProcessInput(window, deltaTime);
			currentTest->OnUpdate(deltaTime);
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