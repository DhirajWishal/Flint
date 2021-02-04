// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/RenderTargets/Pipelines/VulkanGraphicsPipeline.h"
#include "VulkanBackend/VulkanShaderModule.h"
#include "VulkanBackend/VulkanUtilities.h"
#include "VulkanBackend/VulkanMacros.h"

#include "VulkanBackend/RenderTargets/VulkanScreenBoundRenderTargetS.h"

namespace Flint
{
	namespace VulkanBackend
	{
		namespace _Helpers
		{
			VkPrimitiveTopology GetPrimitiveTopology(Backend::PrimitiveTopology topology)
			{
				switch (topology)
				{
				case Flint::Backend::PrimitiveTopology::POINT_LIST:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

				case Flint::Backend::PrimitiveTopology::LINE_LIST:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST;

				case Flint::Backend::PrimitiveTopology::LINE_STRIP:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;

				case Flint::Backend::PrimitiveTopology::TRIANGLE_LIST:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

				case Flint::Backend::PrimitiveTopology::TRIANGLE_STRIP:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

				case Flint::Backend::PrimitiveTopology::TRIANGLE_FAN:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;

				case Flint::Backend::PrimitiveTopology::LINE_LIST_WITH_ADJACENCY:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;

				case Flint::Backend::PrimitiveTopology::LINE_STRIP_WITH_ADJACENCY:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;

				case Flint::Backend::PrimitiveTopology::TRIANGLE_LIST_WITH_ADJACENCY:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;

				case Flint::Backend::PrimitiveTopology::TRIANGLE_STRIP_WITH_ADJACENCY:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;

				case Flint::Backend::PrimitiveTopology::PATCH_LIST:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;

				default:
					FLINT_LOG_ERROR(TEXT("Invalid or Undefined primitive topology!"))
						break;
				}

				return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
			}

			VkCullModeFlags GetCullMode(Backend::CullMode cull)
			{
				switch (cull)
				{
				case Flint::Backend::CullMode::NONE:
					return VkCullModeFlagBits::VK_CULL_MODE_NONE;

				case Flint::Backend::CullMode::FRONT:
					return VkCullModeFlagBits::VK_CULL_MODE_FRONT_BIT;

				case Flint::Backend::CullMode::BACK:
					return VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;

				case Flint::Backend::CullMode::FRONT_AND_BACK:
					return VkCullModeFlagBits::VK_CULL_MODE_FRONT_AND_BACK;

				default:
					FLINT_LOG_ERROR(TEXT("Invalid or Undefined cull mode!"))
						break;
				}

				return VkCullModeFlagBits::VK_CULL_MODE_NONE;
			}

			VkFrontFace GetFrontFace(Backend::FrontFace face)
			{
				switch (face)
				{
				case Flint::Backend::FrontFace::COUNTER_CLOCKWISE:
					return VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;

				case Flint::Backend::FrontFace::CLOCKWISE:
					return VkFrontFace::VK_FRONT_FACE_CLOCKWISE;

				default:
					FLINT_LOG_ERROR(TEXT("Invalid or Undefined front face!"))
						break;
				}

				return VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
			}

			VkPolygonMode GetPolygonMode(Backend::PolygonMode mode)
			{
				switch (mode)
				{
				case Flint::Backend::PolygonMode::FILL:
					return VkPolygonMode::VK_POLYGON_MODE_FILL;

				case Flint::Backend::PolygonMode::LINE:
					return VkPolygonMode::VK_POLYGON_MODE_LINE;

				case Flint::Backend::PolygonMode::POINT:
					return VkPolygonMode::VK_POLYGON_MODE_POINT;

				default:
					FLINT_LOG_ERROR(TEXT("Invalid or Undefined polygon mode!"))
						break;
				}

				return VkPolygonMode::VK_POLYGON_MODE_FILL;
			}
		}

		void VulkanGraphicsPipeline::Initialize(Backend::RenderTarget* pRenderTarget, const std::vector<ShaderDigest>& shaderDigests, const Backend::GraphicsPipelineSpecification& spec)
		{
			this->pRenderTarget = pRenderTarget;
			this->mSpec = spec;
			ResolveUniformLayouts(shaderDigests);

			VulkanDevice* pDevice = pRenderTarget->Derive<VulkanDevice>();

			std::vector<VkVertexInputAttributeDescription> vAttributeDesc = {};
			std::vector<VulkanShaderModule> sModules = {};
			std::vector<VkPushConstantRange> vRanges = {};
			VkVertexInputBindingDescription vDesc = {};

			// Create the descriptor set layout.
			{
				std::vector<VkDescriptorSetLayoutBinding> vBindings = {};
				for (auto itr = shaderDigests.begin(); itr != shaderDigests.end(); itr++)
				{
					VulkanShaderModule sModule = {};
					sModule.Initialize(pDevice, *itr);
					vBindings.insert(vBindings.end(), sModule.mBindings.begin(), sModule.mBindings.end());
					vPoolSizes.insert(vPoolSizes.end(), sModule.mPoolSizes.begin(), sModule.mPoolSizes.end());
					vRanges.insert(vRanges.end(), sModule.mConstantRange.begin(), sModule.mConstantRange.end());

					if (itr->mLocation == ShaderLocation::VERTEX)
					{
						vDesc.binding = 0;
						vDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
						vDesc.stride = static_cast<UI32>(Utilities::GetStride(*itr));

						vAttributeDesc = std::move(Utilities::GetInputAttributeDescriptions(*itr));
					}

					INSERT_INTO_VECTOR(sModules, sModule);
				}

				VkDescriptorSetLayoutCreateInfo vCI = {};
				vCI.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
				vCI.flags = VK_NULL_HANDLE;
				vCI.pNext = VK_NULL_HANDLE;
				vCI.bindingCount = static_cast<UI32>(vBindings.size());
				vCI.pBindings = vBindings.data();

				FLINT_VK_ASSERT(pDevice->CreateDescriptorSetLayout(&vCI, &vSetLayout), "Failed to create descriptor set layout!")
			}

			VkPipelineLayoutCreateInfo vLayoutCI = {};
			vLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			vLayoutCI.flags = VK_NULL_HANDLE;
			vLayoutCI.pNext = VK_NULL_HANDLE;
			vLayoutCI.pushConstantRangeCount = static_cast<UI32>(vRanges.size());
			vLayoutCI.pPushConstantRanges = vRanges.data();
			vLayoutCI.setLayoutCount = 1;
			vLayoutCI.pSetLayouts = &vSetLayout;

			FLINT_VK_ASSERT(pDevice->CreatePipelineLayout(&vLayoutCI, &vPipelineLayout), "Failed to create descriptor set layout!")

				VkPipelineVertexInputStateCreateInfo vVISCI = {};
			vVISCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vVISCI.flags = VK_NULL_HANDLE;
			vVISCI.pNext = VK_NULL_HANDLE;
			vVISCI.vertexAttributeDescriptionCount = static_cast<UI32>(vAttributeDesc.size());
			vVISCI.pVertexAttributeDescriptions = vAttributeDesc.data();
			vVISCI.vertexBindingDescriptionCount = 1;
			vVISCI.pVertexBindingDescriptions = &vDesc;

			VkPipelineInputAssemblyStateCreateInfo vIASCI = {};
			vIASCI.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			vIASCI.flags = VK_NULL_HANDLE;
			vIASCI.pNext = VK_NULL_HANDLE;
			vIASCI.primitiveRestartEnable = GET_VK_BOOL(mSpec.bEnablePrimitiveRestart);
			vIASCI.topology = _Helpers::GetPrimitiveTopology(mSpec.mPrimitiveTopology);

			VkPipelineRasterizationStateCreateInfo vRSCI = {};
			vRSCI.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			vRSCI.flags = VK_NULL_HANDLE;
			vRSCI.pNext = VK_NULL_HANDLE;
			vRSCI.cullMode = _Helpers::GetCullMode(mSpec.mCullMode);
			vRSCI.depthBiasEnable = GET_VK_BOOL(mSpec.bEnableDepthBias);
			vRSCI.depthBiasClamp = mSpec.mDepthBiasFactor;
			vRSCI.depthBiasConstantFactor = mSpec.mDepthConstantFactor;
			vRSCI.depthBiasSlopeFactor = mSpec.mDepthSlopeFactor;
			vRSCI.depthClampEnable = GET_VK_BOOL(mSpec.bEnableDepthClamp);
			vRSCI.frontFace = _Helpers::GetFrontFace(mSpec.mFrontFace);
			vRSCI.lineWidth = mSpec.mRasterizerLineWidth;
			vRSCI.polygonMode = _Helpers::GetPolygonMode(mSpec.mPolygonMode);
			vRSCI.rasterizerDiscardEnable = GET_VK_BOOL(mSpec.bEnableRasterizerDiscard);

			// Finals.
			for (auto itr = sModules.begin(); itr != sModules.end(); itr++)
				itr->Terminate();
		}

		void VulkanGraphicsPipeline::Terminate()
		{
			VulkanDevice* pDevice = pRenderTarget->Derive<VulkanDevice>();
			pDevice->DestroyPipelineLayout(vPipelineLayout);
			pDevice->DestroyDescriptorSetLayout(vSetLayout);
		}
	}
}