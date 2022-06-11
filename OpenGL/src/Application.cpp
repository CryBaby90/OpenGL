#include "glad/glad.h"
#include "GLFW/glfw3.h"

#include <iostream>

#include "Application.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

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
    glViewport(0, 0, 800, 600);
    //拖拽回调 改变渲染窗口大小
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    //渲染循环
    while (!glfwWindowShouldClose(window))
    {
        //输入
        processInput(window);

        //渲染指令
        glClearColor(0.2f, 0.3f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);//清屏函数  GL_COLOR_BUFFER_BIT清理颜色缓冲

        // 检查并调用事件，交换缓冲
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //关闭
    glfwTerminate();
	return 0;
}