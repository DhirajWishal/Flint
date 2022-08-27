// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/Backend/Device.hpp"
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
		 * @param frameWidth The camera frame width.
		 * @param frameWHeight The camera frame height.
		 */
		explicit MonoCamera(const glm::vec3 position, const uint32_t frameWidth, const uint32_t frameHeight);

		/**
		 * Update the matrices.
		 */
		void update() override;

		/**
		 * Create a uniform buffer required to store the camera's matrix.
		 *
		 * @param pDevice The device pointer.
		 * @return The buffer pointer.
		 */
		[[nodiscard]] std::shared_ptr<Backend::Buffer> createBuffer(const std::shared_ptr<Backend::Device>& pDevice) const;

		/**
		 * Copy the matrix to a uniform buffer.
		 *
		 * @param pBuffer The buffer to copy the data to.
		 */
		void copyToBuffer(const std::shared_ptr<Backend::Buffer>& pBuffer) const;

	public:
		Matrix m_Matrix;
	};
}