// Copyright 2021 Dhiraj Wishal
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
			glm::mat4 mViewMatrix = glm::mat4(1.0f);
			glm::mat4 mProjectionMatrix = glm::mat4(1.0f);
		};

		struct CameraVectors
		{
			glm::vec3 mPosition = glm::vec3(0.0f);
			glm::vec3 mFront = glm::vec3(0.0f, 0.0f, 1.0f);
			glm::vec3 mRight = glm::vec3(1.0f, 0.0f, 0.0f);
			glm::vec3 mUp = glm::vec3(0.0f, 1.0f, 0.0f);
			glm::vec3 mWorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
		};

		struct Camera : public ComponentBase
		{
			Camera(const std::string_view& componentName) : ComponentBase(componentName) {}

			ViewProjectionMatrix mViewProjection;
			CameraVectors mCameraVectors;

			float mFieldOfView = 60.0f;
			float mAspectRatio = 0.5f;
			float mCameraFar = 256.0f;
			float mCameraNear = 0.001f;

			float mYaw = 90.0f;
			float mPitch = 0.0f;

			void Update();
		};
	}
}