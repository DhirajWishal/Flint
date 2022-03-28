// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ComponentBase.hpp"
#include <glm/glm.hpp>

namespace Flint
{
	namespace Components
	{
		struct ViewProjectionMatrix
		{
			glm::mat4 m_ViewMatrix = glm::mat4(1.0f);
			glm::mat4 m_ProjectionMatrix = glm::mat4(1.0f);
		};

		struct CameraVectors
		{
			glm::vec3 m_Position = glm::vec3(0.0f);
			glm::vec3 m_Front = glm::vec3(0.0f, 0.0f, 1.0f);
			glm::vec3 m_Right = glm::vec3(1.0f, 0.0f, 0.0f);
			glm::vec3 m_Up = glm::vec3(0.0f, 1.0f, 0.0f);
			glm::vec3 m_WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
		};

		struct Camera
		{
			ViewProjectionMatrix m_ViewProjection;
			CameraVectors m_CameraVectors;

			float m_FieldOfView = 60.0f;
			float m_AspectRatio = 0.5f;
			float m_CameraFar = 256.0f;
			float m_CameraNear = 0.001f;

			float m_Yaw = 90.0f;
			float m_Pitch = 0.0f;

			void Update();
		};
	}
}