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
			 * Vertex input structure.
			 * This contains information about a single vertex input.
			 */
			struct VertexInput final
			{
				VkFormat m_Format = VK_FORMAT_UNDEFINED;
				VertexAttribute m_Attribute = VertexAttribute::Max;
			};

			/**
			 * Instance input structure.
			 * This structure contains information about a single instance input attribute.
			 */
			struct InstanceInput final
			{
				VkFormat m_Format = VK_FORMAT_UNDEFINED;
				InstanceAttribute m_Attribute = InstanceAttribute::Max;
			};

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
			 * Get the shader stage create info structures.
			 *
			 * @return The structure vector.
			 */
			[[nodiscard]] const std::vector<VkPipelineShaderStageCreateInfo>& getPipelineShaderStageCreateInfos() const { return m_ShaderStageCreateInfos; }

		private:
			/**
			 * Create a shader module.
			 *
			 * @param shader The shader source.
			 * @param stageFlags The shader stage flags.
			 * @param bindings The layout bindings.
			 * @param poolSizes The pool sizes.
			 * @param pushConstants The shader push constants.
			 * @return The created shader module.
			 */
			[[nodiscard]] VkShaderModule createShaderModule(const ShaderCode& shader, VkShaderStageFlags stageFlags, std::vector<VkDescriptorSetLayoutBinding>& bindings, std::vector<VkDescriptorPoolSize>& poolSizes, std::vector<VkPushConstantRange>& pushConstants);

			/**
			 * Create the descriptor set layout.
			 *
			 * @param bindings The descriptor set layout bindings.
			 */
			void createDescriptorSetLayout(std::vector<VkDescriptorSetLayoutBinding>&& bindings);

			/**
			 * Create the pipeline layouts.
			 *
			 * @param pushConstants The push constants used by the shader.
			 */
			void createPipelineLayout(std::vector<VkPushConstantRange>&& pushConstants);

		private:
			SparseArray<VkDescriptorSet> m_DescriptorSets;
			std::vector<VertexInput> m_VertexInputs;
			std::vector<InstanceInput> m_InstanceInputs;
			std::vector<VkPipelineShaderStageCreateInfo> m_ShaderStageCreateInfos;

			VkShaderModule m_VertexShaderModule = VK_NULL_HANDLE;
			VkShaderModule m_FragmentShaderModule = VK_NULL_HANDLE;

			VkDescriptorSetLayout m_DescriptorSetLayout = VK_NULL_HANDLE;
			VkPipelineLayout m_PipelineLayout = VK_NULL_HANDLE;
			VkDescriptorPool m_DescriptorPool = VK_NULL_HANDLE;
		};
	}
}