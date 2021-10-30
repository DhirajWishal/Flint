// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GraphicsCore/Device.hpp"
#include <glm/glm.hpp>

namespace Flint
{
	/**
	 * Flint editor camera matrix.
	 */
	struct CameraMatrix
	{
		CameraMatrix(const glm::mat4& view, const glm::mat4& projection) : mViewMatrix(view), mProjectionMatrix(projection) {}

		glm::mat4 mViewMatrix = glm::mat4(1);
		glm::mat4 mProjectionMatrix = glm::mat4(1);
	};

	/**
	 * Flint editor camera.
	 */
	class Camera
	{
	public:
		Camera();

		/**
		 * Move the camera to the front.
		 */
		void MoveFront(uint64 delta);

		/**
		 * Move the camera to the back.
		 */
		void MoveBack(uint64 delta);

		/**
		 * Move the camera to the left.
		 */
		void MoveLeft(uint64 delta);

		/**
		 * Move the camera to the right.
		 */
		void MoveRight(uint64 delta);

		/**
		 * Handle mouse position.
		 *
		 * @param position The mouse position.
		 */
		void MousePosition(FExtent2D<float> position);

		/**
		 * Update the camera.
		 * This updates all the vectors and matrices.
		 */
		void Update(uint64 delta);

		/**
		 * Get the default view projection matrices.
		 *
		 * @return The camera matrix.
		 */
		CameraMatrix GetMatrix() const { return CameraMatrix(viewMatrix, projectionMatrix); }

		/**
		 * Reset the first mouse boolean value to its default (true).
		 */
		void ResetFirstMouse();

		/**
		 * Set the camera aspect ratio.
		 *
		 * @param extent The display extent.
		 */
		void SetAspectRatio(FBox2D extent);

	public:
		/**
		 * Get the camera position.
		 *
		 * @return The position vector.
		 */
		const glm::vec3 GetPosition() const { return mPosition; }

		/**
		 * Set the camera position.
		 *
		 * @param position The camera position.
		 */
		void SetPosition(glm::vec3 position) { mPosition = position; }

		/**
		 * Get the camera up vector.
		 *
		 * @return The camera up vector.
		 */
		const glm::vec3 GetCameraUp() const { return mUp; }

		/**
		 * Get the camera front vector.
		 *
		 * @return The camera front vector.
		 */
		const glm::vec3 GetCameraFront() const { return mFront; }

		/**
		 * Set the camera's range (far and near plane).
		 *
		 * @param near The near plane.
		 * @param far The far plane.
		 */
		void SetCameraRange(float near, float far) { mCameraNear = near, mCameraFar = far; }

		/**
		 * Get the camera range.
		 *
		 * @return The near and far plane.
		 */
		const std::pair<float, float> GetCameraRange() const { return { mCameraNear, mCameraFar }; }

		/**
		 * Get the camera pitch and yaw values.
		 *
		 * @return The rotation values.
		 */
		const std::pair<float, float> GetPitchYaw() const { return { mPitch, mYaw }; }

		/**
		 * Set the camera view matrix.
		 *
		 * @param mat The matrix to set.
		 */
		void SetViewMatrix(glm::mat4 mat) { viewMatrix = mat; }

	private:
		glm::mat4 viewMatrix = glm::mat4(1);
		glm::mat4 projectionMatrix = glm::mat4(1);

		glm::vec3 mPosition = glm::vec3{ 0.0f, 1.0f, 0.0f };
		glm::vec3 mUp = glm::vec3{ 0.0f, 1.0f, 0.0f };
		glm::vec3 mFront = glm::vec3{ 0.0f, 0.0f, -1.0f };
		glm::vec3 mRight = glm::vec3{ 1.0f, 0.0f, 0.0f };
		glm::vec3 mWorldUp = glm::vec3{ 0.0f, 1.0f, 0.0f };

		float mMovementBias = 0.05f;

		float mFieldOfView = 60.0f;
		float mAspectRatio = 0.5f;
		float mCameraFar = 256.0f;
		float mCameraNear = 0.001f;

		float mAngelX = 0.0f;
		float mAngelY = 0.0f;
		float mAngelZ = 0.0f;

		float mLastX = 0.0f;
		float mLastY = 0.0f;

		float mYaw = 90.0f;
		float mPitch = 0.0f;

		uint32 mWindowWidth = 0;
		uint32 mWindowHeight = 0;

		bool bFirstMouse = true;
	};
}