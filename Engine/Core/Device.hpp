// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Instance.hpp"
#include "Types.hpp"

#include <vector>
#include <filesystem>

namespace Flint
{
	namespace Core
	{
		/**
		 * Device class.
		 * This class contains everything that's needed for a single device instance.
		 *
		 * Usually an application requires just one instance of this class. But the user isn't restricted to do that in any way. But make sure that the devices are destroyed before
		 * destroying the instance which is used to create this.
		 */
		class Device : public FObject
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pInstance The instance to create the device with.
			 */
			explicit Device(const std::shared_ptr<Instance>& pInstance) : m_pInstance(pInstance) {}

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
			[[nodiscard]] Instance& getInstance() { return *m_pInstance; }

			/**
			 * Get the instance.
			 *
			 * @return The const instance reference.
			 */
			[[nodiscard]] const Instance& getInstance() const { return *m_pInstance; }

			/**
			 * Create a new texture image.
			 *
			 * @param path The path to the texture file.
			 * @param usage The image usage.
			 */
			[[nodiscard]] virtual ImageHandle createTextureImage(std::filesystem::path&& path, ImageUsage usage) = 0;

		private:
			std::shared_ptr<Instance> m_pInstance = nullptr;
		};
	}
}