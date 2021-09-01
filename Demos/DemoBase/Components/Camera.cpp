// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Camera.hpp"
#include "Flint/Device.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

void Camera::Initialize(std::shared_ptr<Flint::Device> pDevice)
{
	pCameraBuffer = pDevice->CreateBuffer(Flint::BufferType::Uniform, sizeof(CameraMatrix));
	this->pDevice = pDevice;

	projectionMatrix = glm::perspective(glm::radians(fieldOfView), aspectRatio, cameraNear, cameraFar);
	projectionMatrix[1][1] *= -1.0f;
}

void Camera::Terminate()
{
	pCameraBuffer->Terminate();
}

void Camera::WalkUp(UI64 delta)
{
	cameraPosition += cameraFront * (static_cast<float>(delta) / 100000000);
}

void Camera::WalkDown(UI64 delta)
{
	cameraPosition -= cameraFront * (static_cast<float>(delta) / 100000000);
}

void Camera::WalkLeft(UI64 delta)
{
	cameraPosition -= cameraRight * (static_cast<float>(delta) / 100000000);
}

void Camera::WalkRight(UI64 delta)
{
	cameraPosition += cameraRight * (static_cast<float>(delta) / 100000000);
}

void Camera::MousePosition(Flint::FExtent2D<float> _pos, UI64 delta)
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
}

void Camera::Update(UI64 delta)
{
	glm::vec3 front;
	front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	front.y = sin(glm::radians(Pitch));
	front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
	cameraFront = glm::normalize(front);
	cameraRight = glm::normalize(glm::cross(cameraFront, worldUp));
	cameraUp = glm::normalize(glm::cross(cameraRight, cameraFront));

	viewMatrix = glm::lookAt(cameraPosition, cameraPosition + cameraFront, cameraUp);

	CameraMatrix* pMatrix = static_cast<CameraMatrix*>(pCameraBuffer->MapMemory(sizeof(CameraMatrix)));
	*pMatrix = GetMatrix();
	pCameraBuffer->UnmapMemory();

	//ImGui::Text("Camera");
	//ImGui::InputFloat3("front", &cameraFront.x);
	//ImGui::InputFloat3("right", &cameraRight.x);
}

void Camera::ResetFirstMouse()
{
	firstMouse = true;
}

void Camera::SetAspectRatio(Flint::FBox2D extent)
{
	aspectRatio = static_cast<float>(extent.mWidth) / static_cast<float>(extent.mHeight);
}