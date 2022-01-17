// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Camera.hpp"

#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>

namespace Flint
{
	Camera::Camera()
	{
		projectionMatrix = glm::perspective(glm::radians(mFieldOfView), mAspectRatio, mCameraNear, mCameraFar);
		projectionMatrix[1][1] *= -1.0f;
	}

	void Camera::MoveFront(uint64 delta)
	{
		mPosition += mFront * (static_cast<float>(delta) / 100000000) * mMovementBias;
	}

	void Camera::MoveBack(uint64 delta)
	{
		mPosition -= mFront * (static_cast<float>(delta) / 100000000) * mMovementBias;
	}

	void Camera::MoveLeft(uint64 delta)
	{
		mPosition -= mRight * (static_cast<float>(delta) / 100000000) * mMovementBias;
	}

	void Camera::MoveRight(uint64 delta)
	{
		mPosition += mRight * (static_cast<float>(delta) / 100000000) * mMovementBias;
	}

	void Camera::MousePosition(FExtent2D<float> position)
	{
		position.X *= -1.0f;
		position.Y *= -1.0f;

		if (bFirstMouse)
		{
			mLastX = position.X;
			mLastY = position.Y;
			bFirstMouse = false;
		}

		float xoffset = position.X - mLastX;
		float yoffset = mLastY - position.Y; // reversed since y-coordinates go from bottom to top

		float sensitivity = 0.05f;
		xoffset *= sensitivity * 0.75f;
		yoffset *= sensitivity;

		mLastX = position.X;
		mLastY = position.Y;

		mYaw += xoffset;
		mPitch += yoffset;

		if (mPitch > 89.0f)
			mPitch = 89.0f;
		if (mPitch < -89.0f)
			mPitch = -89.0f;
	}

	void Camera::Update()
	{
		glm::vec3 front;
		front.x = cos(glm::radians(mYaw)) * cos(glm::radians(mPitch));
		front.y = sin(glm::radians(mPitch));
		front.z = sin(glm::radians(mYaw)) * cos(glm::radians(mPitch));
		mFront = glm::normalize(front);
		mRight = glm::normalize(glm::cross(mFront, mWorldUp));
		mUp = glm::normalize(glm::cross(mRight, mFront));

		viewMatrix = glm::lookAt(mPosition, mPosition + mFront, mUp);

		ImGui::Begin("Camera");
		ImGui::InputFloat3("Position ", &mPosition.x);
		ImGui::InputFloat3("Front ", &mFront.x);
		ImGui::InputFloat3("Right ", &mRight.x);
		ImGui::InputFloat3("Up ", &mUp.x);
		ImGui::InputFloat("Pitch ", &mPitch);
		ImGui::InputFloat("Yaw ", &mYaw);
		ImGui::SliderFloat("Movement Bias", &mMovementBias, 0.005f, 1.0f);
		ImGui::End();

		projectionMatrix = glm::perspective(glm::radians(mFieldOfView), mAspectRatio, mCameraNear, mCameraFar);
		projectionMatrix[1][1] *= -1.0f;
	}

	void Camera::ResetFirstMouse()
	{
		bFirstMouse = true;
	}

	void Camera::SetAspectRatio(FBox2D extent)
	{
		mAspectRatio = static_cast<float>(extent.mWidth) / static_cast<float>(extent.mHeight);
		projectionMatrix = glm::perspective(glm::radians(mFieldOfView), mAspectRatio, mCameraNear, mCameraFar);
		projectionMatrix[1][1] *= -1.0f;
	}

	float Camera::GetAspectRatio() const
	{
		return mAspectRatio;
	}
}
