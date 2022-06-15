// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/StaticModel.hpp"
#include "VulkanVertexStorage.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		/**
		 * Vulkan static model class.
		 */
		class VulkanStaticModel final : public Core::StaticModel<VulkanDevice>
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param device The device reference.
			 * @param assetFile The asset file to load the data from.
			 */
			explicit VulkanStaticModel(VulkanDevice& device, std::filesystem::path&& assetFile);

			/**
			 * Destructor.
			 */
			~VulkanStaticModel() override;

			/**
			 * Terminate the object.
			 */
			void terminate() override;

		private:
			VulkanVertexStorage m_VertexStorage;
			std::unique_ptr<VulkanBuffer> m_pIndexBuffer = nullptr;
		};
	}
}