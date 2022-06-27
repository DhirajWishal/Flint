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
		class VulkanStaticModel final : public StaticModel
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device reference.
			 * @param assetFile The asset file to load the data from.
			 */
			explicit VulkanStaticModel(const std::shared_ptr<VulkanDevice>& pDevice, std::filesystem::path&& assetFile);

			/**
			 * Destructor.
			 */
			~VulkanStaticModel() override;

			/**
			 * Terminate the object.
			 */
			void terminate() override;

		private:
			/**
			 * Load the model data.
			 */
			void loadData();

		private:
			VulkanVertexStorage m_VertexStorage;
			std::shared_ptr<VulkanBuffer> m_pIndexBuffer = nullptr;
		};
	}
}