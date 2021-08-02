// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Backend/Core/DataType.hpp"
#include <glm/glm.hpp>

struct CameraMatrix
{
	CameraMatrix(const glm::mat4& view, const glm::mat4& projection) : mViewMatrix(view), mProjectionMatrix(projection) {}

	glm::mat4 mViewMatrix = glm::mat4(1);
	glm::mat4 mProjectionMatrix = glm::mat4(1);
};

/**
 * Basic camera object for debugging.
 */
class Camera
{
public:
	Camera() = default;

	/**
	 * Walk the camera up.
	 */
	void WalkUp();

	/**
	 * Walk the camera down.
	 */
	void WalkDown();

	/**
	 * Walk the camera left.
	 */
	void WalkLeft();

	/**
	 * Walk the camera right.
	 */
	void WalkRight();

	/**
	 * Handle mouse position.
	 *
	 * @param _pos: The mouse position.
	 */
	void MousePosition(Flint::FExtent2D<float> _pos);

	/**
	 * Update the camera.
	 * This updates all the vectors and matrices.
	 */
	void Update();

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
	 * @param extent: The display extent.
	 */
	void SetAspectRatio(Flint::FBox2D extent);

private:
	glm::mat4 viewMatrix = glm::mat4(1);
	glm::mat4 projectionMatrix = glm::mat4(1);

	glm::vec3 cameraPosition = { 0.0f, 1.0f, 0.0f };
	glm::vec3 cameraUp = { 0.0f, 1.0f, 0.0f };
	glm::vec3 cameraFront = { 0.0f, 0.0f, -1.0f };
	glm::vec3 cameraRight = { 1.0f, 0.0f, 0.0f };
	glm::vec3 worldUp = glm::vec3(0.0f, 1.0f, 0.0f);

	float movementBias = 0.01f;

	float fieldOfView = 60.0f;
	float aspectRatio = 0.5f;
	float cameraFar = 256.0f;
	float cameraNear = 0.001f;

	float angelX = 0.0f;
	float angelY = 0.0f;
	float angelZ = 0.0f;

	float lastX = 0.0f;
	float lastY = 0.0f;

	float Yaw = 90.0f;
	float Pitch = 0.0f;

	UI32 windowWidth = 0;
	UI32 windowHeight = 0;

	bool firstMouse = true;
};