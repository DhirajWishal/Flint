// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Camera/Camera.hpp"

namespace Flint
{
	Camera::Camera(const glm::vec3 position, const uint32_t frameWidth, const uint32_t frameHeight, const float fieldOfView /*= 60.0f*/)
		: m_Position(position)
		, m_FrameWidth(frameWidth)
		, m_FrameHeight(frameHeight)
		, m_AspectRatio(static_cast<float>(frameWidth) / frameHeight)
		, m_FieldOfView(fieldOfView)
	{
	}

	void Camera::moveForward(const uint64_t delta)
	{
		m_Position += m_Front * (static_cast<float>(delta) / m_DeltaReductionFactor) * m_MovementBias;
	}

	void Camera::moveBackward(const uint64_t delta)
	{
		m_Position -= m_Front * (static_cast<float>(delta) / m_DeltaReductionFactor) * m_MovementBias;
	}

	void Camera::moveLeft(const uint64_t delta)
	{
		m_Position -= m_Right * (static_cast<float>(delta) / m_DeltaReductionFactor) * m_MovementBias;
	}

	void Camera::moveRight(const uint64_t delta)
	{
		m_Position += m_Right * (static_cast<float>(delta) / m_DeltaReductionFactor) * m_MovementBias;
	}

	void Camera::moveUp(const uint64_t delta)
	{
		m_Position += m_Up * (static_cast<float>(delta) / m_DeltaReductionFactor) * m_MovementBias;
	}

	void Camera::moveDown(const uint64_t delta)
	{
		m_Position -= m_Up * (static_cast<float>(delta) / m_DeltaReductionFactor) * m_MovementBias;
	}

	void Camera::rotateUp(const uint64_t delta)
	{
		m_Pitch += (static_cast<float>(delta) / m_DeltaReductionFactor) * m_RotationBias;
	}

	void Camera::rotateDown(const uint64_t delta)
	{
		m_Pitch -= (static_cast<float>(delta) / m_DeltaReductionFactor) * m_RotationBias;
	}

	void Camera::rotateLeft(const uint64_t delta)
	{
		m_Yaw += (static_cast<float>(delta) / m_DeltaReductionFactor) * m_RotationBias;
	}

	void Camera::rotateRight(const uint64_t delta)
	{
		m_Yaw -= (static_cast<float>(delta) / m_DeltaReductionFactor) * m_RotationBias;
	}
}