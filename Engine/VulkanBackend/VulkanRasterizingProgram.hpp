// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/RasterizingProgram.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		/**
		 * Vulkan rasterizing program class.
		 */
		class VulkanRasterizingProgram final : public Core::RasterizingProgram<VulkanDevice>
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param device The device to which the program is bound to.
			 * @param vertexShader The vertex shader source file.
			 * @param fragmentShader The fragment shader source file.
			 */
			explicit VulkanRasterizingProgram(VulkanDevice& device, std::filesystem::path&& vertexShader, std::filesystem::path&& fragmetShader);

			/**
			 * Destructor.
			 */
			~VulkanRasterizingProgram() override;

		private:
			SparseArray<VkDescriptorSet> m_DescriptorSets;
			std::vector<VkDescriptorSetLayout> m_SetLayouts;

			VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
			VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
		};
	}
}