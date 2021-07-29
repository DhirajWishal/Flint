// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanGraphicsPipeline.hpp"
#include "VulkanBackend/VulkanShader.hpp"
#include "VulkanBackend/VulkanScreenBoundRenderTarget.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		namespace _Helpers
		{
			VkFormat GetFormatFromSize(ShaderAttributeDataType type)
			{
				switch (type)
				{
				case Flint::ShaderAttributeDataType::VEC1:
					return VkFormat::VK_FORMAT_R32_SFLOAT;

				case Flint::ShaderAttributeDataType::VEC2:
					return VkFormat::VK_FORMAT_R32G32_SFLOAT;

				case Flint::ShaderAttributeDataType::VEC3:
					return VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT;

				default:
					FLINT_THROW_BACKEND_ERROR("Invalid or unsupported shader attribute type!");
				}

				return VkFormat::VK_FORMAT_UNDEFINED;
			}

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
					FLINT_THROW_BACKEND_ERROR("Invalid or Undefined primitive topology!");
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
					FLINT_THROW_BACKEND_ERROR("Invalid or Undefined cull mode!");
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
					FLINT_THROW_BACKEND_ERROR("Invalid or Undefined front face!");
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
					FLINT_THROW_BACKEND_ERROR("Invalid or Undefined polygon mode!");
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
					FLINT_THROW_BACKEND_ERROR("Invalid or Undefined color blend logic!");
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
					FLINT_THROW_BACKEND_ERROR("Invalid or Undefined depth compare logic!");
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

		VulkanGraphicsPipeline::VulkanGraphicsPipeline(
			const std::shared_ptr<Device>& pDevice,
			const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget,
			const std::shared_ptr<Shader>& pVertexShader,
			const std::shared_ptr<Shader>& pTessellationControlShader,
			const std::shared_ptr<Shader>& pTessellationEvaluationShader,
			const std::shared_ptr<Shader>& pGeometryShader,
			const std::shared_ptr<Shader>& pFragmentShader,
			const GraphicsPipelineSpecification& specification)
			: GraphicsPipeline(
				pDevice,
				pScreenBoundRenderTarget,
				pVertexShader,
				pTessellationControlShader,
				pTessellationEvaluationShader,
				pGeometryShader,
				pFragmentShader,
				specification)
		{
			FLINT_SETUP_PROFILER();

			vRenderPass = pScreenBoundRenderTarget->StaticCast<VulkanScreenBoundRenderTarget>().GetRenderPass();

			// Resolve shader stages.
			INSERT_INTO_VECTOR(vShaderStageCreateInfo, pVertexShader->StaticCast<VulkanShader>().GetShaderStageCreateInfo());
			INSERT_INTO_VECTOR(vShaderStageCreateInfo, pFragmentShader->StaticCast<VulkanShader>().GetShaderStageCreateInfo());

			if (pTessellationControlShader)
				INSERT_INTO_VECTOR(vShaderStageCreateInfo, pTessellationControlShader->StaticCast<VulkanShader>().GetShaderStageCreateInfo());

			if (pTessellationEvaluationShader)
				INSERT_INTO_VECTOR(vShaderStageCreateInfo, pTessellationEvaluationShader->StaticCast<VulkanShader>().GetShaderStageCreateInfo());

			if (pGeometryShader)
				INSERT_INTO_VECTOR(vShaderStageCreateInfo, pGeometryShader->StaticCast<VulkanShader>().GetShaderStageCreateInfo());

			// Resolve vertex input state.
			{
				VkVertexInputAttributeDescription vAttributeDescription = {};
				VkVertexInputBindingDescription vBindingDescription = {};
				const auto inputAttributes = pVertexShader->GetInputAttributes();

				for (const auto binding : inputAttributes)
				{
					vAttributeDescription.binding = binding.first;
					vAttributeDescription.offset = 0;
					for (const auto attribute : binding.second)
					{
						vAttributeDescription.location = attribute.mLocation;
						vAttributeDescription.format = _Helpers::GetFormatFromSize(attribute.mDataType);

						INSERT_INTO_VECTOR(vVertexAttributes, vAttributeDescription);
						vAttributeDescription.offset += static_cast<UI32>(attribute.mDataType);
					}

					vBindingDescription.binding = binding.first;
					vBindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
					vBindingDescription.stride = vAttributeDescription.offset;
					INSERT_INTO_VECTOR(vVertexBindings, vBindingDescription);
				}
			}

			vVertexInputStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
			vVertexInputStateCreateInfo.pNext = VK_NULL_HANDLE;
			vVertexInputStateCreateInfo.flags = 0;
			vVertexInputStateCreateInfo.vertexAttributeDescriptionCount = static_cast<UI32>(vVertexAttributes.size());
			vVertexInputStateCreateInfo.pVertexAttributeDescriptions = vVertexAttributes.data();
			vVertexInputStateCreateInfo.vertexBindingDescriptionCount = static_cast<UI32>(vVertexBindings.size());
			vVertexInputStateCreateInfo.pVertexBindingDescriptions = vVertexBindings.data();

			// Input assembly state.
			vInputAssemblyStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
			vInputAssemblyStateCreateInfo.pNext = VK_NULL_HANDLE;
			vInputAssemblyStateCreateInfo.flags = 0;
			vInputAssemblyStateCreateInfo.primitiveRestartEnable = GET_VK_BOOL(mSpecification.bEnablePrimitiveRestart);
			vInputAssemblyStateCreateInfo.topology = _Helpers::GetPrimitiveTopology(mSpecification.mPrimitiveTopology);

			// Tessellation state.
			vTessellationStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_TESSELLATION_STATE_CREATE_INFO;
			vTessellationStateCreateInfo.pNext = VK_NULL_HANDLE;
			vTessellationStateCreateInfo.flags = 0;
			vTessellationStateCreateInfo.patchControlPoints = mSpecification.mTessellationPatchControlPoints;

			// Rasterization state.
			vRasterizationStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
			vRasterizationStateCreateInfo.pNext = VK_NULL_HANDLE;
			vRasterizationStateCreateInfo.flags = 0;
			vRasterizationStateCreateInfo.cullMode = _Helpers::GetCullMode(mSpecification.mCullMode);
			vRasterizationStateCreateInfo.depthBiasEnable = GET_VK_BOOL(mSpecification.bEnableDepthBias);
			vRasterizationStateCreateInfo.depthBiasClamp = mSpecification.mDepthBiasFactor;
			vRasterizationStateCreateInfo.depthBiasConstantFactor = mSpecification.mDepthConstantFactor;
			vRasterizationStateCreateInfo.depthBiasSlopeFactor = mSpecification.mDepthSlopeFactor;
			vRasterizationStateCreateInfo.depthClampEnable = GET_VK_BOOL(mSpecification.bEnableDepthClamp);
			vRasterizationStateCreateInfo.frontFace = _Helpers::GetFrontFace(mSpecification.mFrontFace);
			vRasterizationStateCreateInfo.lineWidth = mSpecification.mRasterizerLineWidth;
			vRasterizationStateCreateInfo.polygonMode = _Helpers::GetPolygonMode(mSpecification.mPolygonMode);
			vRasterizationStateCreateInfo.rasterizerDiscardEnable = GET_VK_BOOL(mSpecification.bEnableRasterizerDiscard);

			// Multisample state.
			vMultisampleStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
			vMultisampleStateCreateInfo.pNext = VK_NULL_HANDLE;
			vMultisampleStateCreateInfo.flags = 0;
			vMultisampleStateCreateInfo.alphaToCoverageEnable = GET_VK_BOOL(mSpecification.bEnableAlphaCoverage);
			vMultisampleStateCreateInfo.alphaToOneEnable = GET_VK_BOOL(mSpecification.bEnableAlphaToOne);
			vMultisampleStateCreateInfo.minSampleShading = mSpecification.mMinSampleShading;
			vMultisampleStateCreateInfo.pSampleMask;	// TODO
			//vMultisampleStateCreateInfo.rasterizationSamples = VkSampleCountFlagBits::VK_SAMPLE_COUNT_1_BIT;
			vMultisampleStateCreateInfo.rasterizationSamples = static_cast<VkSampleCountFlagBits>(pDevice->StaticCast<VulkanDevice>().GetSampleCount());
			vMultisampleStateCreateInfo.sampleShadingEnable = GET_VK_BOOL(mSpecification.bEnableSampleShading);

			// Depth stencil state.
			vDepthStencilStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
			vDepthStencilStateCreateInfo.pNext = VK_NULL_HANDLE;
			vDepthStencilStateCreateInfo.flags = 0;
			vDepthStencilStateCreateInfo.back.compareOp = VK_COMPARE_OP_ALWAYS;
			vDepthStencilStateCreateInfo.depthTestEnable = GET_VK_BOOL(mSpecification.bEnableDepthTest);
			vDepthStencilStateCreateInfo.depthWriteEnable = GET_VK_BOOL(mSpecification.bEnableDepthWrite);
			vDepthStencilStateCreateInfo.depthCompareOp = _Helpers::GetCompareOp(mSpecification.mDepthCompareLogic);

			CreatePipelineLayout();
			CreatePipeline();
		}

		void VulkanGraphicsPipeline::Recreate(const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget)
		{
			FLINT_SETUP_PROFILER();

			vRenderPass = pScreenBoundRenderTarget->StaticCast<VulkanScreenBoundRenderTarget>().GetRenderPass();

			vkDestroyPipeline(pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), vPipeline, nullptr);
			CreatePipeline();
		}

		void VulkanGraphicsPipeline::Terminate()
		{
			VulkanDevice& vDevice = pDevice->StaticCast<VulkanDevice>();
			vkDestroyPipeline(vDevice.GetLogicalDevice(), vPipeline, nullptr);
			vkDestroyPipelineLayout(vDevice.GetLogicalDevice(), vPipelineLayout, nullptr);
		}

		void VulkanGraphicsPipeline::CreatePipelineLayout()
		{
			FLINT_SETUP_PROFILER();

			std::vector<VkDescriptorSetLayout> vSetLayouts;

			INSERT_INTO_VECTOR(vSetLayouts, pVertexShader->StaticCast<VulkanShader>().GetLayout());
			INSERT_INTO_VECTOR(vSetLayouts, pFragmentShader->StaticCast<VulkanShader>().GetLayout());

			if (pTessellationControlShader)
				INSERT_INTO_VECTOR(vSetLayouts, pTessellationControlShader->StaticCast<VulkanShader>().GetLayout());

			if (pTessellationEvaluationShader)
				INSERT_INTO_VECTOR(vSetLayouts, pTessellationEvaluationShader->StaticCast<VulkanShader>().GetLayout());

			if (pGeometryShader)
				INSERT_INTO_VECTOR(vSetLayouts, pGeometryShader->StaticCast<VulkanShader>().GetLayout());

			VkPipelineLayoutCreateInfo vCI = {};
			vCI.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
			vCI.pNext = VK_NULL_HANDLE;
			vCI.flags = 0;
			vCI.pushConstantRangeCount = 0;
			vCI.pPushConstantRanges = nullptr;
			vCI.setLayoutCount = static_cast<UI32>(vSetLayouts.size());
			vCI.pSetLayouts = vSetLayouts.data();

			FLINT_VK_ASSERT(vkCreatePipelineLayout(pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), &vCI, nullptr, &vPipelineLayout));
		}

		void VulkanGraphicsPipeline::CreatePipeline()
		{
			FLINT_SETUP_PROFILER();

			// Resolve viewport state.
			VkRect2D vR2D = {};
			vR2D.extent.width = static_cast<UI32>(pRenderTarget->GetExtent().mWidth);
			vR2D.extent.height = static_cast<UI32>(pRenderTarget->GetExtent().mHeight);
			vR2D.offset = { 0, 0 };

			VkViewport vVP = {};
			vVP.width = static_cast<float>(vR2D.extent.width);
			vVP.height = static_cast<float>(vR2D.extent.height);
			vVP.maxDepth = 1.0f;
			vVP.minDepth = 0.0f;
			vVP.x = 0.0f;
			vVP.y = 0.0f;

			VkPipelineViewportStateCreateInfo vViewportStateCreateInfo = {};
			vViewportStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
			vViewportStateCreateInfo.pNext = VK_NULL_HANDLE;
			vViewportStateCreateInfo.flags = 0;
			vViewportStateCreateInfo.scissorCount = 1;
			vViewportStateCreateInfo.pScissors = &vR2D;
			vViewportStateCreateInfo.viewportCount = 1;
			vViewportStateCreateInfo.pViewports = &vVP;

			// Color blend state.
			VkPipelineColorBlendAttachmentState vCBAS = {};
			vCBAS.colorWriteMask = 0xf;
			vCBAS.blendEnable = GET_VK_BOOL(mSpecification.bEnableColorBlend);

			VkPipelineColorBlendStateCreateInfo vColorBlendStateCreateInfo = {};
			vColorBlendStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
			vColorBlendStateCreateInfo.pNext = VK_NULL_HANDLE;
			vColorBlendStateCreateInfo.flags = 0;
			vColorBlendStateCreateInfo.logicOp = _Helpers::GetLogicOp(mSpecification.mColorBlendLogic);
			vColorBlendStateCreateInfo.logicOpEnable = GET_VK_BOOL(mSpecification.bEnableColorBlendLogic);
			std::copy(mSpecification.mColorBlendConstants, mSpecification.mColorBlendConstants + 4, vColorBlendStateCreateInfo.blendConstants);
			vColorBlendStateCreateInfo.pAttachments = &vCBAS;
			vColorBlendStateCreateInfo.attachmentCount = 1;

			// Dynamic state.
			std::vector<VkDynamicState> vDynamicStates = std::move(_Helpers::GetDynamicStates(mSpecification.mDynamicStateFlags));

			VkPipelineDynamicStateCreateInfo vDynamicStateCreateInfo = {};
			vDynamicStateCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
			vDynamicStateCreateInfo.pNext = VK_NULL_HANDLE;
			vDynamicStateCreateInfo.flags = 0;
			vDynamicStateCreateInfo.dynamicStateCount = static_cast<UI32>(vDynamicStates.size());
			vDynamicStateCreateInfo.pDynamicStates = vDynamicStates.data();

			// Pipeline create info.
			VkGraphicsPipelineCreateInfo vCreateInfo = {};
			vCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
			vCreateInfo.pNext = VK_NULL_HANDLE;
			vCreateInfo.flags = 0;
			vCreateInfo.stageCount = static_cast<UI32>(vShaderStageCreateInfo.size());
			vCreateInfo.pStages = vShaderStageCreateInfo.data();
			vCreateInfo.pVertexInputState = &vVertexInputStateCreateInfo;
			vCreateInfo.pInputAssemblyState = &vInputAssemblyStateCreateInfo;
			vCreateInfo.pTessellationState = &vTessellationStateCreateInfo;
			vCreateInfo.pViewportState = &vViewportStateCreateInfo;
			vCreateInfo.pRasterizationState = &vRasterizationStateCreateInfo;
			vCreateInfo.pMultisampleState = &vMultisampleStateCreateInfo;
			vCreateInfo.pDepthStencilState = &vDepthStencilStateCreateInfo;
			vCreateInfo.pColorBlendState = &vColorBlendStateCreateInfo;
			vCreateInfo.pDynamicState = &vDynamicStateCreateInfo;
			vCreateInfo.layout = vPipelineLayout;
			vCreateInfo.renderPass = vRenderPass;
			vCreateInfo.subpass = 0;	// TODO
			vCreateInfo.basePipelineHandle = VK_NULL_HANDLE;
			vCreateInfo.basePipelineIndex = 0;

			// TODO Pipeline cache

			FLINT_VK_ASSERT(vkCreateGraphicsPipelines(pDevice->StaticCast<VulkanDevice>().GetLogicalDevice(), VK_NULL_HANDLE, 1, &vCreateInfo, nullptr, &vPipeline));
		}
	}
}

/*
Vulkan Validation Layer (Validation): Validation Error: [ VUID-VkPipelineColorBlendStateCreateInfo-logicOpEnable-00606 ] Object 0: handle = 0x299e1222618, type = VK_OBJECT_TYPE_DEVICE; | MessageID = 0x234f6296 | Invalid Pipeline CreateInfo[0]: If logic operations feature not enabled, logicOpEnable must be VK_FALSE. The Vulkan spec states: If the logic operations feature is not enabled, logicOpEnable must be VK_FALSE (https://vulkan.lunarg.com/doc/view/1.2.162.1/windows/1.2-extensions/vkspec.html#VUID-VkPipelineColorBlendStateCreateInfo-logicOpEnable-00606)
Vulkan Validation Layer (Validation): Validation Error: [ VUID-VkPipelineMultisampleStateCreateInfo-alphaToOneEnable-00785 ] Object 0: handle = 0x299e1222618, type = VK_OBJECT_TYPE_DEVICE; | MessageID = 0x8fe75dbf | vkCreateGraphicsPipelines() pCreateInfo[0]: the alphaToOne device feature is disabled: the alphaToOneEnable member of the VkPipelineMultisampleStateCreateInfo structure must be set to VK_FALSE. The Vulkan spec states: If the alpha to one feature is not enabled, alphaToOneEnable must be VK_FALSE (https://vulkan.lunarg.com/doc/view/1.2.162.1/windows/1.2-extensions/vkspec.html#VUID-VkPipelineMultisampleStateCreateInfo-alphaToOneEnable-00785)
*/