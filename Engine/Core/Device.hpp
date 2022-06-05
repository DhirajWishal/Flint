// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.hpp"
#include "Types.hpp"

#include <vector>
#include <filesystem>

namespace Flint
{
	/**
	 * Engine class.
	 * This class contains everything that's needed for a single device instance.
	 *
	 * Usually an application requires just one instance of this class. But the user isn't restricted to do that in any way. But make sure that the devices are destroyed before
	 * destroying the instance which is used to create this.
	 */
	template<class TInstance>
	class Device : public FObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param instance The instance to create the device with.
		 */
		explicit Device(TInstance& instance) : m_Instance(instance) {}

		/**
		 * Default virtual destructor.
		 */
		virtual ~Device() = default;

		/**
		 * Halt the device till all the commands and everything else is executed.
		 */
		virtual void waitIdle() = 0;

		/**
		 * Get the best depth pixel format.
		 *
		 * @return The pixel format.
		 */
		[[nodiscard]] virtual PixelFormat getBestDepthFormat() const = 0;

		/**
		 * Get the max supported multisample count.
		 *
		 * @return The multisample count.
		 */
		[[nodiscard]] virtual Multisample getMaximumMultisample() const = 0;

		/**
		 * Get the instance.
		 *
		 * @return The instance reference.
		 */
		[[nodiscard]] TInstance& getInstance() { return m_Instance; }

		/**
		 * Get the instance.
		 *
		 * @return The const instance reference.
		 */
		[[nodiscard]] const TInstance& getInstance() const { return m_Instance; }

		/**
		 * Create a new buffer.
		 *
		 * @param size The size of the buffer.
		 * @param usage The buffer usage.
		 * @return The buffer handle.
		 */
		[[nodiscard]] virtual BufferHandle createBuffer(uint64_t size, BufferUsage usage) = 0;

		/**
		 * Copy data to a buffer.
		 *
		 * @param handle The buffer handle.
		 * @param pData The data to copy.
		 * @param size The size of the data to copy.
		 * @param offset The buffer's offset to copy to.
		 */
		[[nodiscard]] virtual void copyToBuffer(BufferHandle handle, const std::byte* pData, uint64_t size, uint64_t offset = 0) = 0;

		/**
		 * Create a new texture image.
		 *
		 * @param path The path to the texture file.
		 * @param usage The image usage.
		 */
		[[nodiscard]] virtual ImageHandle createTextureImage(std::filesystem::path&& path, ImageUsage usage) = 0;

	private:
		TInstance& m_Instance;
	};
}