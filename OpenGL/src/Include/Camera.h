#pragma once
#include "GLCall.h"

#include <glm/glm.hpp>

class Camera
{
public:
	Camera(glm::vec3 cameraPos);
	~Camera();

	void OnProcessMouseMovement(GLfloat xoffset, GLfloat yoffset, GLboolean constrainPitch = true);
	void OnScroll(GLfloat xoffset, GLfloat yoffset);
	void OnProcessInput(GLFWwindow* window, GLfloat deltaTime);
	void SetFov(GLfloat fov);

	glm::mat4 GetViewMatrix();
	glm::mat4 GetProjMatrix();

private:
	glm::vec3 m_CameraPos, m_CameraFront, m_CameraUp;
	GLfloat m_Yaw, m_Pitch, m_Speed, m_Zoom, m_Fov;
	//¸©Ñö½Ç(Pitch) ÉÏÏÂ
	//Æ«º½½Ç(Yaw) ×óÓÒ
	//¹ö×ª½Ç(Roll) ·­¹ö
};
