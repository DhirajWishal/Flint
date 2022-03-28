// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Engine/Components/Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>

namespace Flint
{
	namespace Components
	{
		void Camera::Update()
		{
			glm::vec3 front;
			front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
			front.y = sin(glm::radians(m_Pitch));
			front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
			m_CameraVectors.m_Front = glm::normalize(front);
			m_CameraVectors.m_Right = glm::normalize(glm::cross(m_CameraVectors.m_Front, m_CameraVectors.m_WorldUp));
			m_CameraVectors.m_Up = glm::normalize(glm::cross(m_CameraVectors.m_Right, m_CameraVectors.m_Front));

			m_ViewProjection.m_ViewMatrix = glm::lookAt(m_CameraVectors.m_Position, m_CameraVectors.m_Position + m_CameraVectors.m_Front, m_CameraVectors.m_Up);
			m_ViewProjection.m_ProjectionMatrix = glm::perspective(glm::radians(m_FieldOfView), m_AspectRatio, m_CameraNear, m_CameraFar);
			m_ViewProjection.m_ProjectionMatrix[1][1] *= -1.0f;
		}
	}
}