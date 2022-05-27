// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Engine.hpp"
#include "Camera.hpp"

namespace Flint
{
	/**
	 * Mono camera class.
	 * This contains a single camera.
	 */
	class MonoCamera final : public Camera
	{
		/**
		 * Matrix structure.
		 * This structure is used to store all the matrix information.
		 */
		struct Matrix final
		{
			glm::mat4 m_View = glm::mat4(1.0f);
			glm::mat4 m_Projection = glm::mat4(1.0f);
		};

	public:
		/**
		 * Constructor.
		 *
		 * @param position The camera position in the 3D world.
		 * @pram aspectRatio The camera's aspect ratio.
		 */
		explicit MonoCamera(const glm::vec3 position, const float aspectRatio);

		/**
		 * Update the matrices.
		 */
		void update() override;

		/**
		 * Copy the matrix to a uniform buffer.
		 *
		 * @param engine The engine which owns the buffer.
		 * @param buffer The buffer to copy the data to.
		 */
		void copyToBuffer(Engine& engine, BufferHandle buffer) const;

	public:
		Matrix m_Matrix;
	};
}