#include "Application.h"

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
    /*Shader shader("res/shaders/Basic.shader");
    shader.Bind();*/
    ShaderSource source = ParseShader("res/shaders/Vertex.Vshader", "res/shaders/Fragment.Fshader");
    std::cout << source.VertexSource << std::endl;
    std::cout << source.FragmentSource << std::endl;
    unsigned int shaderProgram = CreateShader(source.VertexSource, source.FragmentSource);

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


        //  ��������
        GLCall(glUseProgram(shaderProgram));
        GLCall(glBindVertexArray(VAO));
        GLCall(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0)); 

        GLCall(glBindVertexArray(0));

        // ��鲢�����¼�����������
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //��Դ�ͷ�
    GLCall(glDeleteVertexArrays(1, &VAO));
    GLCall(glDeleteBuffers(1, &VBO));
    GLCall(glDeleteBuffers(1, &EBO));
    GLCall(glDeleteProgram(shaderProgram));

    //�ر�
    glfwTerminate();
	return 0;
}