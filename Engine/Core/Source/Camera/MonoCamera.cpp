// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Camera/MonoCamera.hpp"
#include "Core/Buffer.hpp"

#include <Optick.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Flint
{
	MonoCamera::MonoCamera(const glm::vec3 position, const uint32_t frameWidth, const uint32_t frameHeight)
		: Camera(position, frameWidth, frameHeight)
	{
		m_Matrix.m_Projection = glm::perspective(glm::radians(m_FieldOfView), m_AspectRatio, m_NearPlane, m_FarPlane);
		m_Matrix.m_Projection[1][1] *= -1.0f;
	}

	void MonoCamera::update()
	{
		OPTICK_EVENT();

		// Recalculate the vectors.
		glm::vec3 front;
		front.x = cos(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		front.y = sin(glm::radians(m_Pitch));
		front.z = sin(glm::radians(m_Yaw)) * cos(glm::radians(m_Pitch));
		m_Front = glm::normalize(front);
		m_Right = glm::normalize(glm::cross(m_Front, m_WorldUp));
		m_Up = glm::normalize(glm::cross(m_Right, m_Front));

		// Calculate the matrices.
		m_Matrix.m_View = glm::lookAt(m_Position, m_Position + m_Front, m_Up);
		m_Matrix.m_Projection = glm::perspective(glm::radians(m_FieldOfView), m_AspectRatio, m_NearPlane, m_FarPlane);
		m_Matrix.m_Projection[1][1] *= -1.0f;
	}

	std::shared_ptr<Flint::Buffer> MonoCamera::createBuffer(const std::shared_ptr<Device>& pDevice) const
	{
		return pDevice->createBuffer(sizeof(Matrix), BufferUsage::Uniform);
	}

	void MonoCamera::copyToBuffer(const std::shared_ptr<Buffer>& pBuffer) const
	{
		OPTICK_EVENT();

		auto pMemory = pBuffer->mapMemory();

		std::copy_n(reinterpret_cast<const std::byte*>(&m_Matrix), sizeof(Matrix), pMemory);
		pBuffer->unmapMemory();
	}

}