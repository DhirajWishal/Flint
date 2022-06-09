// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Device.hpp"
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
		 * Copy the matrix to a uniform buffer.
		 *
		 * @param device The device which owns the buffer.
		 * @param buffer The buffer to copy the data to.
		 */
		 //template<class TDevice>
		 //void copyToBuffer(TDevice& device, Core::BufferHandle buffer) const
		 //{
		 //	device.copyToBuffer(buffer, reinterpret_cast<const std::byte*>(&m_Matrix), sizeof(Matrix));
		 //}

	public:
		Matrix m_Matrix;
	};
}