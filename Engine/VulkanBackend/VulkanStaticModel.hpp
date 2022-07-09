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

			/**
			 * Get the input binding descriptions for this model.
			 *
			 * @param mesh To mesh to get the descriptions from.
			 * @param inputs The inputs that we want to access.
			 * @reutrn The binding descriptions.
			 */
			[[nodiscard]] std::vector<VkVertexInputBindingDescription> getInputBindingDescriptions(const StaticMesh& mesh, const std::vector<VertexInput>& inputs) const;

			/**
			 * Get the input attribute descriptions for this model.
			 *
			 * @param mesh To mesh to get the descriptions from.
			 * @param inputs The inputs that we want to access.
			 * @reutrn The attribute descriptions.
			 */
			[[nodiscard]] std::vector<VkVertexInputAttributeDescription> getInputAttributeDescriptions(const StaticMesh& mesh, const std::vector<VertexInput>& inputs) const;

			/**
			 * Get the vertex storage.
			 *
			 * @return The vertex storage.
			 */
			[[nodiscard]] const VulkanVertexStorage& getVertexStorage() const { return m_VertexStorage; }

			/**
			 * Get the index buffer handle.
			 *
			 * @return The buffer handle.
			 */
			[[nodiscard]] const VkBuffer getIndexBufferHandle() const { return m_pIndexBuffer->getBuffer(); }

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