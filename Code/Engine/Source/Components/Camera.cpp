// Copyright 2021 Dhiraj Wishal
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
			front.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
			front.y = sin(glm::radians(mPitch));
			front.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
			mCameraVectors.mFront = glm::normalize(front);
			mCameraVectors.mRight = glm::normalize(glm::cross(mCameraVectors.mFront, mCameraVectors.mWorldUp));
			mCameraVectors.mUp = glm::normalize(glm::cross(mCameraVectors.mRight, mCameraVectors.mFront));

			mViewProjection.mViewMatrix = glm::lookAt(mCameraVectors.mPosition, mCameraVectors.mPosition + mCameraVectors.mFront, mCameraVectors.mUp);
			mViewProjection.mProjectionMatrix = glm::perspective(glm::radians(mFieldOfView), mAspectRatio, mCameraNear, mCameraFar);
			mViewProjection.mProjectionMatrix[1][1] *= -1.0f;
		}
	}
}