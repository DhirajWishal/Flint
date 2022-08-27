// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint/Backend/RasterizingProgram.hpp"
#include "VulkanDevice.hpp"

#include <unordered_map>

namespace Flint
{
	namespace Backend
	{
		/**
		 * Vulkan rasterizing program class.
		 */
		class VulkanRasterizingProgram final : public RasterizingProgram
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param pDevice The device to which the program is bound to.
			 * @param vertexShader The vertex shader source.
			 * @param fragmentShader The fragment shader source.
			 */
			explicit VulkanRasterizingProgram(const std::shared_ptr<VulkanDevice>& pDevice, ShaderCode&& vertexShader, ShaderCode&& fragmetShader);

			/**
			 * Destructor.
			 */
			~VulkanRasterizingProgram() override;

			/**
			 * Terminate the object.
			 */
			void terminate() override;

			/**
			 * Get the pipeline layout.
			 *
			 * @return The pipeline layout.
			 */
			[[nodiscard]] VkPipelineLayout getPipelineLayout() const { return m_PipelineLayout; }

			/**
			 * Get the descriptor set layout.
			 *
			 * @return The descriptor set layout.
			 */
			[[nodiscard]] VkDescriptorSetLayout getDescriptorSetLayout() const { return m_DescriptorSetLayout; }

			/**
			 * Get the shader stage create info structures.
			 *
			 * @return The structure vector.
			 */
			[[nodiscard]] const std::vector<VkPipelineShaderStageCreateInfo>& getPipelineShaderStageCreateInfos() const { return m_ShaderStageCreateInfos; }

			/**
			 * Get the descriptor layout bindings.
			 *
			 * @return The bindings.
			 */
			[[nodsicard]] const std::vector<VkDescriptorSetLayoutBinding>& getLayoutBindings() const { return m_LayoutBindings; }

			/**
			 * Get the descriptor pool sizes.
			 *
			 * @return The sizes.
			 */
			[[nodsicard]] const std::vector<VkDescriptorPoolSize>& getPoolSizes() const { return m_PoolSizes; }

		private:
			/**
			 * Create a shader module.
			 *
			 * @param shader The shader source.
			 * @param stageFlags The shader stage flags.
			 * @param pushConstants The shader push constants.
			 * @return The created shader module.
			 */
			[[nodiscard]] VkShaderModule createShaderModule(const ShaderCode& shader, VkShaderStageFlags stageFlags, std::vector<VkPushConstantRange>& pushConstants);

			/**
			 * Create the descriptor set layout.
			 */
			void createDescriptorSetLayout();

			/**
			 * Create the pipeline layouts.
			 *
			 * @param pushConstants The push constants used by the shader.
			 */
			void createPipelineLayout(std::vector<VkPushConstantRange>&& pushConstants);

		private:
			SparseArray<VkDescriptorSet> m_DescriptorSets;

			std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStageCreateInfos;
			std::vector<VkDescriptorSetLayoutBinding> m_LayoutBindings;
			std::vector<VkDescriptorPoolSize> m_PoolSizes;

			VkShaderModule m_VertexShaderModule = VK_NULL_HANDLE;
			VkShaderModule m_FragmentShaderModule = VK_NULL_HANDLE;

			VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
			VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
		};
	}
}