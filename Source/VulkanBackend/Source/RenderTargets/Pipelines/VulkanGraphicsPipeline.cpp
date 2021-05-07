// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend\RenderTargets\Pipelines\VulkanGraphicsPipeline.h"
#include "VulkanBackend\RenderTargets\Pipelines\VulkanPipelineResource.h"
#include "VulkanBackend\RenderTargets\VulkanRenderTarget.h"
#include "VulkanBackend\VulkanShaderModule.h"
#include "VulkanBackend\VulkanUtilities.h"
#include "VulkanBackend\VulkanMacros.h"
#include "VulkanBackend\VulkanBuffer.h"

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

			VkFrontFace GetFrontFace(FrontFace face)
			{
				switch (face)
				{
				case Flint::FrontFace::COUNTER_CLOCKWISE:
					return VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;

				case Flint::FrontFace::CLOCKWISE:
					return VkFrontFace::VK_FRONT_FACE_CLOCKWISE;

				default:
					FLINT_LOG_ERROR(TEXT("Invalid or Undefined front face!"))
						break;
				}

				return VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
			}

			VkPolygonMode GetPolygonMode(PolygonMode mode)
			{
				switch (mode)
				{
				case Flint::PolygonMode::FILL:
					return VkPolygonMode::VK_POLYGON_MODE_FILL;

				case Flint::PolygonMode::LINE:
					return VkPolygonMode::VK_POLYGON_MODE_LINE;

				case Flint::PolygonMode::POINT:
					return VkPolygonMode::VK_POLYGON_MODE_POINT;

				default:
					FLINT_LOG_ERROR(TEXT("Invalid or Undefined polygon mode!"))
						break;
				}

				return VkPolygonMode::VK_POLYGON_MODE_FILL;
			}

			VkLogicOp GetLogicOp(ColorBlendLogic logic)
			{
				switch (logic)
				{
				case Flint::ColorBlendLogic::CLEAR:
					return VkLogicOp::VK_LOGIC_OP_CLEAR;

				case Flint::ColorBlendLogic::AND:
					return VkLogicOp::VK_LOGIC_OP_AND;

				case Flint::ColorBlendLogic::AND_REVERSE:
					return VkLogicOp::VK_LOGIC_OP_AND_REVERSE;

				case Flint::ColorBlendLogic::COPY:
					return VkLogicOp::VK_LOGIC_OP_COPY;

				case Flint::ColorBlendLogic::AND_INVERTED:
					return VkLogicOp::VK_LOGIC_OP_AND_INVERTED;

				case Flint::ColorBlendLogic::NO_OP:
					return VkLogicOp::VK_LOGIC_OP_NO_OP;

				case Flint::ColorBlendLogic::XOR:
					return VkLogicOp::VK_LOGIC_OP_XOR;

				case Flint::ColorBlendLogic::OR:
					return VkLogicOp::VK_LOGIC_OP_OR;

				case Flint::ColorBlendLogic::NOR:
					return VkLogicOp::VK_LOGIC_OP_NOR;

				case Flint::ColorBlendLogic::EQUIVALENT:
					return VkLogicOp::VK_LOGIC_OP_EQUIVALENT;

				case Flint::ColorBlendLogic::INVERT:
					return VkLogicOp::VK_LOGIC_OP_INVERT;

				case Flint::ColorBlendLogic::OR_REVERSE:
					return VkLogicOp::VK_LOGIC_OP_OR_REVERSE;

				case Flint::ColorBlendLogic::COPY_INVERTED:
					return VkLogicOp::VK_LOGIC_OP_COPY_INVERTED;

				case Flint::ColorBlendLogic::OR_INVERTED:
					return VkLogicOp::VK_LOGIC_OP_OR_INVERTED;

				case Flint::ColorBlendLogic::NAND:
					return VkLogicOp::VK_LOGIC_OP_NAND;

				case Flint::ColorBlendLogic::SET:
					return VkLogicOp::VK_LOGIC_OP_SET;

				default:
					FLINT_LOG_ERROR(TEXT("Invalid or Undefined color blend logic!"))
						break;
				}

				return VkLogicOp::VK_LOGIC_OP_CLEAR;
			}

			VkCompareOp GetCompareOp(DepthCompareLogic logic)
			{
				switch (logic)
				{
				case Flint::DepthCompareLogic::NEVER:
					return VkCompareOp::VK_COMPARE_OP_NEVER;

				case Flint::DepthCompareLogic::LESS:
					return VkCompareOp::VK_COMPARE_OP_LESS;

				case Flint::DepthCompareLogic::EQUAL:
					return VkCompareOp::VK_COMPARE_OP_EQUAL;

				case Flint::DepthCompareLogic::LESS_OR_EQUAL:
					return VkCompareOp::VK_COMPARE_OP_LESS_OR_EQUAL;

				case Flint::DepthCompareLogic::GREATER:
					return VkCompareOp::VK_COMPARE_OP_GREATER;

				case Flint::DepthCompareLogic::NOT_EQUAL:
					return VkCompareOp::VK_COMPARE_OP_NOT_EQUAL;

				case Flint::DepthCompareLogic::GREATER_OR_EQUAL:
					return VkCompareOp::VK_COMPARE_OP_GREATER_OR_EQUAL;

				case Flint::DepthCompareLogic::ALWAYS:
					return VkCompareOp::VK_COMPARE_OP_ALWAYS;

				default:
					FLINT_LOG_ERROR(TEXT("Invalid or Undefined depth compare logic!"))
						break;
				}

				return VkCompareOp::VK_COMPARE_OP_LESS_OR_EQUAL;
			}

			std::vector<VkDynamicState> GetDynamicStates(DynamicStateFlags flags)
			{
				std::vector<VkDynamicState> states;
				if (flags & DynamicStateFlags::VIEWPORT)
					INSERT_INTO_VECTOR(states, VkDynamicState::VK_DYNAMIC_STATE_VIEWPORT);

				if (flags & DynamicStateFlags::SCISSOR)
					INSERT_INTO_VECTOR(states, VkDynamicState::VK_DYNAMIC_STATE_SCISSOR);

				if (flags & DynamicStateFlags::LINE_WIDTH)
					INSERT_INTO_VECTOR(states, VkDynamicState::VK_DYNAMIC_STATE_LINE_WIDTH);

				if (flags & DynamicStateFlags::DEPTH_BIAS)
					INSERT_INTO_VECTOR(states, VkDynamicState::VK_DYNAMIC_STATE_DEPTH_BIAS);

				if (flags & DynamicStateFlags::BLEND_CONSTANTS)
					INSERT_INTO_VECTOR(states, VkDynamicState::VK_DYNAMIC_STATE_BLEND_CONSTANTS);

				if (flags & DynamicStateFlags::DEPTH_BOUNDS)
					INSERT_INTO_VECTOR(states, VkDynamicState::VK_DYNAMIC_STATE_DEPTH_BOUNDS);

				return states;
			}
		}

		VulkanGraphicsPipeline::VulkanGraphicsPipeline(FScreenBoundRenderTarget* pRenderTarget, const std::vector<FShaderDigest>& shaderDigests, const GraphicsPipelineSpecification& spec)
			: FGraphicsPipeline(pRenderTarget, shaderDigests, spec)
		{
			ResolveUniformLayouts(shaderDigests);
			this->pvRenderTarget = pRenderTarget->GetAs<VulkanScreenBoundRenderTargetS>();
			VulkanDevice* pDevice = pRenderTarget->GetDevice()->GetAs<VulkanDevice>();

			std::vector<VkPipelineShaderStageCreateInfo> vStages = {};
			std::vector<VkVertexInputAttributeDescription> vAttributeDesc = {};
			std::vector<VulkanShaderModule> sModules = {};
			std::vector<VkPushConstantRange> vRanges = {};
			VkVertexInputBindingDescription vDesc = {};

			// Create the descriptor set layout.
			{
				std::vector<VkDescriptorSetLayoutBinding> vBindings = {};
				for (auto itr = shaderDigests.begin(); itr != shaderDigests.end(); itr++)
				{
					VulkanShaderModule sModule = { pDevice, *itr };
					INSERT_INTO_VECTOR(vStages, sModule.GetStage());
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

				FLINT_VK_ASSERT(pDevice->CreateDescriptorSetLayout(&vCI, &vSetLayout), "Failed to create descriptor set layout!");
			}

			VkPipelineLayoutCreateInfo vLayoutCI = {};
			vLayoutCI.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			vLayoutCI.flags = VK_NULL_HANDLE;
			vLayoutCI.pNext = VK_NULL_HANDLE;
			vLayoutCI.pushConstantRangeCount = static_cast<UI32>(vRanges.size());
			vLayoutCI.pPushConstantRanges = vRanges.data();
			vLayoutCI.setLayoutCount = 1;
			vLayoutCI.pSetLayouts = &vSetLayout;

			FLINT_VK_ASSERT(pDevice->CreatePipelineLayout(&vLayoutCI, &vPipelineLayout), "Failed to create descriptor set layout!");

			VkPipelineVertexInputStateCreateInfo vVISCI = {};
			vVISCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vVISCI.flags = VK_NULL_HANDLE;
			vVISCI.pNext = VK_NULL_HANDLE;
			vVISCI.vertexAttributeDescriptionCount = static_cast<UI32>(vAttributeDesc.size());
			vVISCI.pVertexAttributeDescriptions = vAttributeDesc.data();
			vVISCI.vertexBindingDescriptionCount = 1;
			vVISCI.pVertexBindingDescriptions = &vDesc;

			vIASCI.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			vIASCI.flags = VK_NULL_HANDLE;
			vIASCI.pNext = VK_NULL_HANDLE;
			vIASCI.primitiveRestartEnable = GET_VK_BOOL(mSpec.bEnablePrimitiveRestart);
			vIASCI.topology = _Helpers::GetPrimitiveTopology(mSpec.mPrimitiveTopology);

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

			VkRect2D vR2D = {};
			vR2D.extent.width = static_cast<UI32>(pRenderTarget->GetExtent().x);
			vR2D.extent.height = static_cast<UI32>(pRenderTarget->GetExtent().y);
			vR2D.offset = { 0, 0 };

			VkViewport vVP = {};
			vVP.width = static_cast<float>(vR2D.extent.width);
			vVP.height = static_cast<float>(vR2D.extent.height);
			vVP.maxDepth = 1.0f;
			vVP.minDepth = 0.0f;
			vVP.x = 0.0f;
			vVP.y = 0.0f;

			vVSCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			vVSCI.pNext = VK_NULL_HANDLE;
			vVSCI.flags = VK_NULL_HANDLE;
			vVSCI.scissorCount = 1;
			vVSCI.pScissors = &vR2D;
			vVSCI.viewportCount = 1;
			vVSCI.pViewports = &vVP;

			vMSCI.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			vMSCI.pNext = VK_NULL_HANDLE;
			vMSCI.flags = VK_NULL_HANDLE;
			vMSCI.alphaToCoverageEnable = GET_VK_BOOL(mSpec.bEnableAlphaCoverage);
			vMSCI.alphaToOneEnable = GET_VK_BOOL(mSpec.bEnableAlphaToOne);
			vMSCI.minSampleShading = mSpec.mMinSampleShading;
			vMSCI.pSampleMask;
			//vMSCI.rasterizationSamples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
			vMSCI.rasterizationSamples = static_cast<VkSampleCountFlagBits>(pDevice->GetSampleCount());
			vMSCI.sampleShadingEnable = GET_VK_BOOL(mSpec.bEnableSampleShading);

			VkPipelineColorBlendAttachmentState vCBAS = {};
			vCBAS.colorWriteMask = 0xf;
			vCBAS.blendEnable = GET_VK_BOOL(mSpec.bEnableColorBlend);

			vCBSCI.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			vCBSCI.pNext = VK_NULL_HANDLE;
			vCBSCI.flags = VK_NULL_HANDLE;
			vCBSCI.logicOp = _Helpers::GetLogicOp(mSpec.mColorBlendLogic);
			vCBSCI.logicOpEnable = GET_VK_BOOL(mSpec.bEnableColorBlendLogic);
			std::copy(mSpec.mColorBlendConstants, mSpec.mColorBlendConstants + 4, vCBSCI.blendConstants);
			vCBSCI.pAttachments = &vCBAS;
			vCBSCI.attachmentCount = 1;

			vDSSCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			vDSSCI.flags = VK_NULL_HANDLE;
			vDSSCI.pNext = VK_NULL_HANDLE;
			vDSSCI.back.compareOp = VK_COMPARE_OP_ALWAYS;
			vDSSCI.depthTestEnable = GET_VK_BOOL(mSpec.bEnableDepthTest);
			vDSSCI.depthWriteEnable = GET_VK_BOOL(mSpec.bEnableDepthWrite);
			vDSSCI.depthCompareOp = _Helpers::GetCompareOp(mSpec.mDepthCompareLogic);

			std::vector<VkDynamicState> vDynamicStates = std::move(_Helpers::GetDynamicStates(mSpec.mDynamicStateFlags));

			VkPipelineDynamicStateCreateInfo vDSCI = {};
			vDSCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			vDSCI.pNext = VK_NULL_HANDLE;
			vDSCI.flags = VK_NULL_HANDLE;
			vDSCI.dynamicStateCount = static_cast<UI32>(vDynamicStates.size());
			vDSCI.pDynamicStates = vDynamicStates.data();

			VkGraphicsPipelineCreateInfo vCI = {};
			vCI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			vCI.flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT;
			vCI.pNext = VK_NULL_HANDLE;
			vCI.pVertexInputState = &vVISCI;
			vCI.pInputAssemblyState = &vIASCI;
			vCI.pRasterizationState = &vRSCI;
			vCI.pViewportState = &vVSCI;
			vCI.pMultisampleState = &vMSCI;
			vCI.pColorBlendState = &vCBSCI;
			vCI.pDepthStencilState = &vDSSCI;
			vCI.pDynamicState = &vDSCI;
			vCI.stageCount = static_cast<UI32>(vStages.size());
			vCI.pStages = vStages.data();
			vCI.renderPass = pvRenderTarget->GetRenderPass();
			vCI.basePipelineHandle = VK_NULL_HANDLE;
			vCI.basePipelineIndex = 0;
			vCI.layout = vPipelineLayout;

			VkPipeline vNewPipeline = VK_NULL_HANDLE;
			FLINT_VK_ASSERT(pDevice->CreateGraphicsPipeline(&vCI, &vNewPipeline), "Failed to create graphics pipeline!");

			if (vPipeline != VK_NULL_HANDLE)
				pRenderTarget->GetDevice()->GetAs<VulkanDevice>()->DestroyPipeline(vPipeline);

			vPipeline = vNewPipeline;

			// Finals.
			for (auto itr = sModules.begin(); itr != sModules.end(); itr++)
				itr->Terminate();
		}

		void VulkanGraphicsPipeline::Terminate()
		{
			VulkanDevice* pDevice = pRenderTarget->GetDevice()->GetAs<VulkanDevice>();
			pDevice->DestroyPipeline(vPipeline);
			pDevice->DestroyPipelineLayout(vPipelineLayout);
			pDevice->DestroyDescriptorSetLayout(vSetLayout);
		}

		void VulkanGraphicsPipeline::PrepareToRecreate()
		{
			VulkanDevice* pDevice = pRenderTarget->GetDevice()->GetAs<VulkanDevice>();
			pDevice->DestroyPipeline(vPipeline);
			pDevice->DestroyPipelineLayout(vPipelineLayout);

			vPipeline = VK_NULL_HANDLE;
			vPipelineLayout = VK_NULL_HANDLE;
		}

		void VulkanGraphicsPipeline::Recreate()
		{
			VulkanDevice* pDevice = pRenderTarget->GetDevice()->GetAs<VulkanDevice>();

			std::vector<VkPipelineShaderStageCreateInfo> vStages = {};
			std::vector<VkVertexInputAttributeDescription> vAttributeDesc = {};
			std::vector<VulkanShaderModule> sModules = {};
			VkVertexInputBindingDescription vDesc = {};

			for (auto itr = mDigests.begin(); itr != mDigests.end(); itr++)
			{
				VulkanShaderModule sModule = { pDevice, *itr };
				INSERT_INTO_VECTOR(vStages, sModule.GetStage());

				if (itr->mLocation == ShaderLocation::VERTEX)
				{
					vDesc.binding = 0;
					vDesc.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
					vDesc.stride = static_cast<UI32>(Utilities::GetStride(*itr));

					vAttributeDesc = std::move(Utilities::GetInputAttributeDescriptions(*itr));
				}

				INSERT_INTO_VECTOR(sModules, sModule);
			}

			VkPipelineVertexInputStateCreateInfo vVISCI = {};
			vVISCI.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vVISCI.flags = VK_NULL_HANDLE;
			vVISCI.pNext = VK_NULL_HANDLE;
			vVISCI.vertexAttributeDescriptionCount = static_cast<UI32>(vAttributeDesc.size());
			vVISCI.pVertexAttributeDescriptions = vAttributeDesc.data();
			vVISCI.vertexBindingDescriptionCount = 1;
			vVISCI.pVertexBindingDescriptions = &vDesc;

			VkRect2D vR2D = {};
			vR2D.extent.width = static_cast<UI32>(pRenderTarget->GetExtent().x);
			vR2D.extent.height = static_cast<UI32>(pRenderTarget->GetExtent().y);
			vR2D.offset = { 0, 0 };

			VkViewport vVP = {};
			vVP.width = static_cast<float>(vR2D.extent.width);
			vVP.height = static_cast<float>(vR2D.extent.height);
			vVP.maxDepth = 1.0f;
			vVP.minDepth = 0.0f;
			vVP.x = 0.0f;
			vVP.y = 0.0f;

			vVSCI.scissorCount = 1;
			vVSCI.pScissors = &vR2D;
			vVSCI.viewportCount = 1;
			vVSCI.pViewports = &vVP;

			VkPipelineColorBlendAttachmentState vCBAS = {};
			vCBAS.colorWriteMask = 0xf;
			vCBAS.blendEnable = GET_VK_BOOL(mSpec.bEnableColorBlend);

			vCBSCI.pAttachments = &vCBAS;

			std::vector<VkDynamicState> vDynamicStates = std::move(_Helpers::GetDynamicStates(mSpec.mDynamicStateFlags));

			VkPipelineDynamicStateCreateInfo vDSCI = {};
			vDSCI.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			vDSCI.pNext = VK_NULL_HANDLE;
			vDSCI.flags = VK_NULL_HANDLE;
			vDSCI.dynamicStateCount = static_cast<UI32>(vDynamicStates.size());
			vDSCI.pDynamicStates = vDynamicStates.data();

			VkGraphicsPipelineCreateInfo vCI = {};
			vCI.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			vCI.pNext = VK_NULL_HANDLE;
			vCI.flags = VK_PIPELINE_CREATE_ALLOW_DERIVATIVES_BIT;
			vCI.pVertexInputState = &vVISCI;
			vCI.pInputAssemblyState = &vIASCI;
			vCI.pRasterizationState = &vRSCI;
			vCI.pViewportState = &vVSCI;
			vCI.pMultisampleState = &vMSCI;
			vCI.pColorBlendState = &vCBSCI;
			vCI.pDepthStencilState = &vDSSCI;
			vCI.pDynamicState = &vDSCI;
			vCI.stageCount = static_cast<UI32>(vStages.size());
			vCI.pStages = vStages.data();
			vCI.renderPass = pvRenderTarget->GetRenderPass();
			vCI.basePipelineHandle = vPipeline;
			vCI.basePipelineIndex = 0;
			vCI.layout = vPipelineLayout;

			VkPipeline vNewPipeline = VK_NULL_HANDLE;
			FLINT_VK_ASSERT(pDevice->CreateGraphicsPipeline(&vCI, &vNewPipeline), "Failed to create graphics pipeline!");

			if (vPipeline != VK_NULL_HANDLE) pDevice->DestroyPipeline(vPipeline);
			vPipeline = vNewPipeline;

			// Finals.
			for (auto itr = sModules.begin(); itr != sModules.end(); itr++)
				itr->Terminate();
		}

		FPipelineResource* VulkanGraphicsPipeline::CreatePipelineResource()
		{
			return std::make_shared<VulkanPipelineResource>(FGraphicsPipeline*(this));
		}
	}
}
