#pragma once
#include "GLCall.h"
#include "../test/Test.h"

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

bool showCursor = true;
float MouseSensitivity = 0.1f;

::test::Test* currentTest = nullptr;
::test::TestMenu* testMenu = new test::TestMenu(currentTest);

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	GLCall(glViewport(0, 0, width, height));
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_LEFT_ALT) == GLFW_PRESS 
		&& glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
	{
		showCursor = !showCursor;
		if (showCursor)
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		else
			glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	}
}

void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
	ImGui_ImplGlfw_CursorPosCallback(window, xposIn, yposIn);
	if (currentTest != nullptr)
	{
		GLfloat xpos = static_cast<GLfloat>(xposIn);
		GLfloat ypos = static_cast<GLfloat>(yposIn);

		if (firstMouse)
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		GLfloat xoffset = xpos - lastX;
		GLfloat yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

		lastX = xpos;
		lastY = ypos;
		currentTest->OnProcessMouseMovement(xoffset * MouseSensitivity, yoffset * MouseSensitivity);
	}
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	ImGui_ImplGlfw_ScrollCallback(window, xoffset, yoffset);
	GLfloat xpos = static_cast<GLfloat>(xoffset);
	GLfloat ypos = static_cast<GLfloat>(yoffset);
	if (currentTest != nullptr)
		currentTest->OnScroll(xpos, ypos);
}