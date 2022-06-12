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

	// ���� ImGui context
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	// ���� ImGui style
	ImGui::StyleColorsDark();
	//ImGui::StyleColorsClassic();

	// ���� ImGui Platform/Renderer backends
	const char* glsl_version = "#version 130";
	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init(glsl_version);

	{
		//��������֮��
		GLfloat vertices[] = {
			0.5f, 0.5f, 0.0f,   // ���Ͻ�
			0.5f, -0.5f, 0.0f,  // ���½�
			-0.5f, -0.5f, 0.0f, // ���½�
			-0.5f, 0.5f, 0.0f   // ���Ͻ�
		};

		GLuint indices[] = {
			// ע��������0��ʼ! 
			// ����������(0,1,2,3)���Ƕ�������vertices���±꣬
			// �����������±��������ϳɾ���
			0, 1, 3, // ��һ��������
			1, 2, 3  // �ڶ���������
		};

		//  ����VAO, VBO, EBO
		GLuint VAO, VBO, EBO;
		GLCall(glGenVertexArrays(1, &VAO));
		GLCall(glGenBuffers(1, &VBO));//����VBO
		GLCall(glGenBuffers(1, &EBO));//����EBO = IBO:index buffer

		//1  ��VAO�������
		GLCall(glBindVertexArray(VAO));

		//2  ��VBO���������飩�������й�OpenGLʹ��
		GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));//���㻺����󣬶��㻺���� GL_ARRAY_BUFFER��һ������Ŀ��
		GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));//�������
																						  //GL_STATIC_DRAW �����ݲ���򼸺�����ı䡣
																						  //GL_DYNAMIC_DRAW�����ݻᱻ�ı�ܶࡣ
																						  //GL_STREAM_DRAW ������ÿ�λ���ʱ����ı䡣

		//3  ��EBO���������飩��һ�����������У���OpenGLʹ�� 
		GLCall(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO));
		GLCall(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

		//4  ���ö�������ָ��
		//�������� layout(location = 0)
		//�������ԵĴ�С
		//����ָ�����ݵ�����
		//�Ƿ�ϣ�����ݱ���׼��(Normalize)�������������ΪGL_TRUE���������ݶ��ᱻӳ�䵽0�������з�����signed������-1����1֮�䡣���ǰ�������ΪGL_FALSE��
		//����(Stride)
		//λ�������ڻ�������ʼλ�õ�ƫ����(Offset)������λ������������Ŀ�ͷ������������0
		GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
		GLCall(glEnableVertexAttribArray(0));

		//  ���VAO  ֮ǰ��Ҫ���VBO EBO ����Ҫ���°�
		GLCall(glBindVertexArray(0));


		//6  ��ɫ������
		Shader shader("res/shaders/Vertex.Vshader", "res/shaders/Fragment.Fshader");
		
		//�߿�ģʽ
		//GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_LINE));
		//Ĭ��ģʽ
		GLCall(glPolygonMode(GL_FRONT_AND_BACK, GL_FILL));

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

			//  ��������
			shader.Bind();
			GLCall(glBindVertexArray(VAO));
			GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

			GLCall(glBindVertexArray(0));

			// ImGui Rendering  
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			// ��鲢�����¼�����������
			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		//��Դ�ͷ�
		GLCall(glDeleteVertexArrays(1, &VAO));
		GLCall(glDeleteBuffers(1, &VBO));
		GLCall(glDeleteBuffers(1, &EBO));
	}

	// Cleanup
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	//�ر�
	glfwDestroyWindow(window);
	glfwTerminate();
	return 0;
}