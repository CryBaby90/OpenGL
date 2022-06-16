#include "Camera.h"
#include "Global.h"

#include <glm/gtc/matrix_transform.hpp>

GLfloat YAW = -90.0f;
GLfloat PITCH = 0.0f;
GLfloat SPEED = 2.5f;
GLfloat ZOOM = 45.0f;
GLfloat FOV = 45.0f;

Camera::Camera(glm::vec3 cameraPos)
	:m_CameraPos(cameraPos), m_CameraFront(glm::vec3(0.0f, 0.0f, -1.0f)), m_CameraUp(glm::vec3(0.0f, 1.0f, 0.0f)),
	m_Yaw(YAW), m_Pitch(PITCH), m_Speed(SPEED), m_Zoom(ZOOM), m_Fov(FOV)
{
}

Camera::~Camera()
{
}

void Camera::OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch)
{
	m_Yaw += xoffset;
	m_Pitch += yoffset;

	// make sure that when pitch is out of bounds, screen doesn't get flipped
	if (constrainPitch)
	{
		if (m_Pitch > 89.0f)
			m_Pitch = 89.0f;
		if (m_Pitch < -89.0f)
			m_Pitch = -89.0f;
	}

	glm::vec3 front;
	//ÉèÖÃdirection
	front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	front.y = sin(glm::radians(m_Pitch));
	front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
	m_CameraFront = glm::normalize(front);
	glm::vec3 Right = glm::normalize(glm::cross(m_CameraFront, glm::vec3(0.0f, 1.0f, 0.0f)));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
	m_CameraUp = glm::normalize(glm::cross(Right, m_CameraFront));
}

void Camera::OnScroll(GLfloat xoffset, GLfloat yoffset)
{
	if (m_Fov >= 1.0f && FOV <= 45.0f)
		m_Fov -= yoffset;
	if (m_Fov <= 1.0f)
		m_Fov = 1.0f;
	if (m_Fov >= 45.0f)
		m_Fov = 45.0f;
}

void Camera::OnProcessInput(GLFWwindow* window, GLfloat deltaTime)
{
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		m_CameraPos += m_Speed * m_CameraFront * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		m_CameraPos -= m_Speed * m_CameraFront * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		m_CameraPos -= glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * m_Speed * deltaTime;
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		m_CameraPos += glm::normalize(glm::cross(m_CameraFront, m_CameraUp)) * m_Speed * deltaTime;
}

void Camera::SetFov(GLfloat fov)
{
	m_Fov = fov;
}

glm::mat4 Camera::GetViewMatrix()
{
	return glm::lookAt(m_CameraPos, m_CameraPos + m_CameraFront, m_CameraUp);
}

glm::mat4 Camera::GetProjMatrix()
{
	return glm::perspective(glm::radians(m_Fov), static_cast<float>(SCR_WIDTH / SCR_HEIGHT), 0.1f, 100.0f);
}
