// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

void Camera::WalkUp()
{
	cameraPosition += cameraFront * movementBias;
}

void Camera::WalkDown()
{
	cameraPosition -= cameraFront * movementBias;
}

void Camera::WalkLeft()
{
	cameraPosition -= cameraRight * movementBias;
}

void Camera::WalkRight()
{
	cameraPosition += cameraRight * movementBias;
}

void Camera::MousePosition(Flint::FExtent2D<float> _pos)
{
	_pos.X *= -1.0f;
	_pos.Y *= -1.0f;
	if (firstMouse)
	{
		lastX = _pos.X;
		lastY = _pos.Y;
		firstMouse = false;
	}

	float xoffset = _pos.X - lastX;
	float yoffset = lastY - _pos.Y; // reversed since y-coordinates go from bottom to top

	float sensitivity = 0.05f;
	xoffset *= sensitivity * 0.75f;
	yoffset *= sensitivity;

	lastX = _pos.X;
	lastY = _pos.Y;

	Yaw += xoffset;
	Pitch += yoffset;

	if (Pitch > 89.0f)
		Pitch = 89.0f;
	if (Pitch < -89.0f)
		Pitch = -89.0f;

	Update();
}

void Camera::Update()
{
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	cameraFront = glm::normalize(front);
	cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
	cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));

	viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);
	projectionMatrix = glm::perspective(glm::radians(fieldOfView), aspectRatio, cameraNear, cameraFar);
	projectionMatrix[1][1] *= -1.0f;
}

void Camera::ResetFirstMouse()
{
	firstMouse = true;
}

void Camera::SetAspectRatio(Flint::FBox2D extent)
{
	aspectRatio = static_cast<float>(extent.mWidth) / static_cast<float>(extent.mHeight);
}

void Camera::Rotate(float degrees, I8 coord)
{
	// Rotate x
	if (coord == 0)
		modelMatrix *= glm::rotate(glm::mat4(1.0f), degrees, glm::vec3(1.0f, 0.0f, 0.0f));

	// Rotate y
	else if (coord == 1)
		modelMatrix *= glm::rotate(glm::mat4(1.0f), degrees, glm::vec3(0.0f, 1.0f, 0.0f));

	// Rotate z
	else if (coord == 2)
		modelMatrix *= glm::rotate(glm::mat4(1.0f), degrees, glm::vec3(0.0f, 0.0f, 1.0f));
}
