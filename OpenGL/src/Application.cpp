#include "Application.h"

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    
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

    //在上下文之后
    GLfloat vertices[] = {
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f,
        0.0f,  0.5f, 0.0f
    };

    //  绑定VAO
    GLuint VAO;
    glGenVertexArrays(1, &VAO);
    GLCall(glBindVertexArray(VAO));

    //  复制顶点数组到缓冲中供OpenGL使用
    GLuint VBO;
    GLCall(glGenBuffers(1, &VBO));//创建VBO
    GLCall(glBindBuffer(GL_ARRAY_BUFFER, VBO));//顶点缓冲对象，顶点缓冲区 GL_ARRAY_BUFFER是一个缓冲目标
    GLCall(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));//填充数据
                                                                                      //GL_STATIC_DRAW ：数据不会或几乎不会改变。
                                                                                      //GL_DYNAMIC_DRAW：数据会被改变很多。
                                                                                      //GL_STREAM_DRAW ：数据每次绘制时都会改变。
    //  设置顶点属性指针
    //顶点属性 layout(location = 0)
    //顶点属性的大小
    //参数指定数据的类型
    //是否希望数据被标准化(Normalize)。如果我们设置为GL_TRUE，所有数据都会被映射到0（对于有符号型signed数据是-1）到1之间。我们把它设置为GL_FALSE。
    //步长(Stride)
    //位置数据在缓冲中起始位置的偏移量(Offset)。由于位置数据在数组的开头，所以这里是0
    GLCall(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0));
    GLCall(glEnableVertexAttribArray(0));
    
    //解绑VAO
    GLCall(glBindVertexArray(0));

    //  着色器程序
    ShaderSource source = ParseShader("res/shaders/Basic.shader");
    std::cout << source.VertexSource << std::endl;
    std::cout << source.FragmentSource << std::endl;
    unsigned int m_ShaderID = CreateShader(source.VertexSource, source.FragmentSource);

    //渲染循环
    while (!glfwWindowShouldClose(window))
    {
        //输入
        processInput(window);

        //渲染指令
        GLCall(glClearColor(0.2f, 0.3f, 0.3f, 1.0f));
        GLCall(glClear(GL_COLOR_BUFFER_BIT));//清屏函数  GL_COLOR_BUFFER_BIT清理颜色缓冲

        
        //  绘制物体
        GLCall(glUseProgram(m_ShaderID));
        GLCall(glBindVertexArray(VAO));
        glDrawArrays(GL_TRIANGLES, 0, 3);
        GLCall(glBindVertexArray(0));

        // 检查并调用事件，交换缓冲
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //关闭
    glfwTerminate();
	return 0;
}