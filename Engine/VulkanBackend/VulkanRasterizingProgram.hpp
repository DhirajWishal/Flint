// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/RasterizingProgram.hpp"
#include "VulkanDevice.hpp"

#include <unordered_map>

namespace Flint
{
	namespace VulkanBackend
	{
		/**
		 * Vulkan rasterizing program class.
		 */
		class VulkanRasterizingProgram final : public RasterizingProgram
		{
			/**
			 * Vertex shader input structure.
			 * This contains information about a single vertex input.
			 */
			struct VertexShaderInput final
			{
				uint32_t m_Location = 0;
				VkFormat m_Format = VK_FORMAT_UNDEFINED;
			};

		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device to which the program is bound to.
			 * @param vertexShader The vertex shader source file.
			 * @param fragmentShader The fragment shader source file.
			 */
			explicit VulkanRasterizingProgram(const std::shared_ptr<VulkanDevice>& pDevice, std::filesystem::path&& vertexShader, std::filesystem::path&& fragmetShader);

			/**
			 * Destructor.
			 */
			~VulkanRasterizingProgram() override;

			/**
			 * Terminate the object.
			 */
			void terminate() override;

		private:
			/**
			 * Create a shader module.
			 *
			 * @param shaderPath The shader source path.
			 * @param stageFlags The shader stage flags.
			 * @param bindingMap The layout binding map.
			 * @param poolSizes The pool sizes.
			 * @param pushConstants The shader push constants.
			 * @return The created shader module.
			 */
			[[nodiscard]] VkShaderModule createShaderModule(const std::filesystem::path& shaderPath, VkShaderStageFlags stageFlags, std::unordered_map<uint32_t, std::vector<VkDescriptorSetLayoutBinding>>& bindingMap, std::vector<VkDescriptorPoolSize>& poolSizes, std::vector<VkPushConstantRange>& pushConstants);

		private:
			SparseArray<VkDescriptorSet> m_DescriptorSets;
			std::vector<VkDescriptorSetLayout> m_SetLayouts;
			std::vector<VertexShaderInput> m_VertexShaderInputs;

			VkShaderModule m_VertexShaderModule = VK_NULL_HANDLE;
			VkShaderModule m_FragmentShaderModule = VK_NULL_HANDLE;

			VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
			VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
		};
	}
}