#include "Application.h"

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    
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

    //��������֮��
    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };

    //  ��VAO
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    GLCall(glBindVertexArray(VAO));

    //  ���ƶ������鵽�����й�OpenGLʹ��
    GLuint VBO;
    GLCall(glGenBuffers(1, &VBO));//����VBO
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));//���㻺����󣬶��㻺���� GL_ARRAY_BUFFER��һ������Ŀ��
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));//�������
                                                                                      //GL_STATIC_DRAW �����ݲ���򼸺�����ı䡣
                                                                                      //GL_DYNAMIC_DRAW�����ݻᱻ�ı�ܶࡣ
                                                                                      //GL_STREAM_DRAW ������ÿ�λ���ʱ����ı䡣
    //  ���ö�������ָ��
    //�������� layout(location = 0)
    //�������ԵĴ�С
    //����ָ�����ݵ�����
    //�Ƿ�ϣ�����ݱ���׼��(Normalize)�������������ΪGL_TRUE���������ݶ��ᱻӳ�䵽0�������з�����signed������-1����1֮�䡣���ǰ�������ΪGL_FALSE��
    //����(Stride)
    //λ�������ڻ�������ʼλ�õ�ƫ����(Offset)������λ������������Ŀ�ͷ������������0
    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
    GLCall(glEnableVertexAttribArray(0));
    
    //���VAO
    GLCall(glBindVertexArray(0));

    //  ��ɫ������
    ShaderSource source = ParseShader("res/shaders/Basic.shader");
    std::cout << source.VertexSource << std::endl;
    std::cout << source.FragmentSource << std::endl;
    unsigned int m_ShaderID = CreateShader(source.VertexSource, source.FragmentSource);

    //��Ⱦѭ��
    while (!glfwWindowShouldClose(window))
    {
        //����
        processInput(window);

        //��Ⱦָ��
        GLCall(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));//��������  GL_COLOR_BUFFER_BIT������ɫ����

        
        //  ��������
        GLCall(glUseProgram(m_ShaderID));
        GLCall(glBindVertexArray(VAO));
        glDrawArrays(GL_TRIANGLES, 0, 3);
        GLCall(glBindVertexArray(0));

        // ��鲢�����¼�����������
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //�ر�
    glfwTerminate();
	return 0;
}