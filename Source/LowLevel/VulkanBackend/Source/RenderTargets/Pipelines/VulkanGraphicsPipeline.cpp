// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/RenderTargets/Pipelines/VulkanGraphicsPipeline.h"
#include "VulkanBackend/VulkanShaderModule.h"
#include "VulkanBackend/VulkanUtilities.h"
#include "VulkanBackend/VulkanMacros.h"

#include "VulkanBackend/RenderTargets/VulkanRenderTargetSB2D.h"
#include "VulkanBackend/RenderTargets/VulkanRenderTargetSB3D.h"

namespace Flint
{
	namespace VulkanBackend
	{
		namespace _Helpers
		{
			VkPrimitiveTopology GetPrimitiveTopology(PrimitiveTopology topology)
			{
				switch (topology)
				{
				case Flint::PrimitiveTopology::POINT_LIST:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_POINT_LIST;

				case Flint::PrimitiveTopology::LINE_LIST:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST;

				case Flint::PrimitiveTopology::LINE_STRIP:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_STRIP;

				case Flint::PrimitiveTopology::TRIANGLE_LIST:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;

				case Flint::PrimitiveTopology::TRIANGLE_STRIP:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP;

				case Flint::PrimitiveTopology::TRIANGLE_FAN:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_FAN;

				case Flint::PrimitiveTopology::LINE_LIST_WITH_ADJACENCY:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST_WITH_ADJACENCY;

				case Flint::PrimitiveTopology::LINE_STRIP_WITH_ADJACENCY:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_STRIP_WITH_ADJACENCY;

				case Flint::PrimitiveTopology::TRIANGLE_LIST_WITH_ADJACENCY:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST_WITH_ADJACENCY;

				case Flint::PrimitiveTopology::TRIANGLE_STRIP_WITH_ADJACENCY:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_STRIP_WITH_ADJACENCY;

				case Flint::PrimitiveTopology::PATCH_LIST:
					return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_PATCH_LIST;

				default:
					FLINT_LOG_ERROR(TEXT("Invalid or Undefined primitive topology!"))
						break;
				}

				return VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_LINE_LIST;
			}

			VkCullModeFlags GetCullMode(CullMode cull)
			{
				switch (cull)
				{
				case Flint::CullMode::NONE:
					return VkCullModeFlagBits::VK_CULL_MODE_NONE;

				case Flint::CullMode::FRONT:
					return VkCullModeFlagBits::VK_CULL_MODE_FRONT_BIT;

				case Flint::CullMode::BACK:
					return VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;

				case Flint::CullMode::FRONT_AND_BACK:
					return VkCullModeFlagBits::VK_CULL_MODE_FRONT_AND_BACK;

				default:
					FLINT_LOG_ERROR(TEXT("Invalid or Undefined cull mode!"))
						break;
				}

				return VkCullModeFlagBits::VK_CULL_MODE_NONE;
			}
		}

		void VulkanGraphicsPipeline::Initialize(VulkanDevice* pDevice, VulkanRenderTarget* pRenderTarget, const std::vector<ShaderDigest>& shaderDigest, const GraphicsPipelineSpecification& spec)
		{
			this->pDevice = pDevice;
			this->mSpec = spec;
			ResolveUniformLayouts(shaderDigest);

			std::vector<VkVertexInputAttributeDescription> vAttributeDesc = {};
			std::vector<VulkanShaderModule> sModules = {};
			std::vector<VkPushConstantRange> vRanges = {};
			VkVertexInputBindingDescription vDesc = {};

			// Create the descriptor set layout.
			{
				std::vector<VkDescriptorSetLayoutBinding> vBindings = {};
				for (auto itr = shaderDigest.begin(); itr != shaderDigest.end(); itr++)
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
			vRSCI.depthBiasClamp;

			// Finals.
			for (auto itr = sModules.begin(); itr != sModules.end(); itr++)
				itr->Terminate();
		}

		void VulkanGraphicsPipeline::Terminate()
		{
			pDevice->DestroyPipelineLayout(vPipelineLayout);
			pDevice->DestroyDescriptorSetLayout(vSetLayout);
		}

		Interface::GraphicsPipelineHandle CreateGraphicsPipelineSB2D(const Interface::RenderTargetHandleSB2D& renderTargetHandle, const std::vector<ShaderDigest>& digests, const GraphicsPipelineSpecification& spec)
		{
			VulkanRenderTargetSB2D* pRenderTarget = static_cast<VulkanRenderTargetSB2D*>(HandleToPointer(renderTargetHandle));
			VulkanGraphicsPipeline* pPipeline = new VulkanGraphicsPipeline();
			pPipeline->Initialize(pRenderTarget->GetDevice(), pRenderTarget, digests, spec);

			return PointerToHandle<Interface::GraphicsPipelineHandle>(pPipeline);
		}

		Interface::GraphicsPipelineHandle CreateGraphicsPipelineSB3D(const Interface::RenderTargetHandleSB3D& renderTargetHandle, const std::vector<ShaderDigest>& digests, const GraphicsPipelineSpecification& spec)
		{
			VulkanRenderTargetSB3D* pRenderTarget = static_cast<VulkanRenderTargetSB3D*>(HandleToPointer(renderTargetHandle));
			VulkanGraphicsPipeline* pPipeline = new VulkanGraphicsPipeline();
			pPipeline->Initialize(pRenderTarget->GetDevice(), pRenderTarget, digests, spec);

			return PointerToHandle<Interface::GraphicsPipelineHandle>(pPipeline);
		}

		Interface::GraphicsPipelineHandle CreateGraphicsPipelineSBRT(const Interface::RenderTargetHandleSBRT& renderTargetHandle, const std::vector<ShaderDigest>& digests, const GraphicsPipelineSpecification& spec)
		{
			return Interface::GraphicsPipelineHandle();
		}

		Interface::GraphicsPipelineHandle CreateGraphicsPipelineOS2D(const Interface::RenderTargetHandleOS2D& renderTargetHandle, const std::vector<ShaderDigest>& digests, const GraphicsPipelineSpecification& spec)
		{
			return Interface::GraphicsPipelineHandle();
		}

		Interface::GraphicsPipelineHandle CreateGraphicsPipelineOS3D(const Interface::RenderTargetHandleOS3D& renderTargetHandle, const std::vector<ShaderDigest>& digests, const GraphicsPipelineSpecification& spec)
		{
			return Interface::GraphicsPipelineHandle();
		}

		Interface::GraphicsPipelineHandle CreateGraphicsPipelineOSRT(const Interface::RenderTargetHandleOSRT& renderTargetHandle, const std::vector<ShaderDigest>& digests, const GraphicsPipelineSpecification& spec)
		{
			return Interface::GraphicsPipelineHandle();
		}

		void DestroyGraphicsPipeline(const Interface::GraphicsPipelineHandle& handle)
		{
			VulkanGraphicsPipeline* pPipeline = static_cast<VulkanGraphicsPipeline*>(HandleToPointer(handle));
			pPipeline->Terminate();

			delete pPipeline;
		}
	}
}