// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanPipeline.h"
#include "Core\Backend\GraphicsPipeline.h"
#include "VulkanBackend\VulkanShaderModule.h"
#include "VulkanBackend\VulkanUtilities.h"
#include "VulkanBackend\VulkanMacros.h"
#include "VulkanBackend\VulkanBuffer.h"
#include "VulkanBackend\VulkanImage.h"
#include "VulkanBackend\RenderTargets\VulkanScreenBoundRenderTargetS.h"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanScreenBoundRenderTargetS;

		template<class TRenderTarget = VulkanScreenBoundRenderTargetS>
		class VulkanGraphicsPipeline final : public VulkanPipeline, public Backend::GraphicsPipeline<VulkanGraphicsPipeline<TRenderTarget>, VulkanDevice, TRenderTarget, VulkanBuffer, VulkanImage, VulkanPipelineResource<VulkanGraphicsPipeline<TRenderTarget>>> {
		public:
			using Super = Backend::GraphicsPipeline<VulkanGraphicsPipeline<TRenderTarget>, VulkanDevice, TRenderTarget, VulkanBuffer, VulkanImage, VulkanPipelineResource<VulkanGraphicsPipeline<TRenderTarget>>>;
			using DeviceType = VulkanDevice;
			using BufferType = VulkanBuffer;
			using ResourceType = VulkanPipelineResource<VulkanGraphicsPipeline<TRenderTarget>>;
			using RenderTargetType = TRenderTarget;
		
		public:
			VulkanGraphicsPipeline() {}
			~VulkanGraphicsPipeline() {}

			virtual void Initialize(RenderTargetType* pRenderTarget, const std::vector<ShaderDigest>& shaderDigests, const Backend::GraphicsPipelineSpecification& spec) override final;
			virtual void PrepareToRecreate() override final;
			virtual void Recreate() override final;
			virtual void Terminate() override final;

			virtual VkPipelineBindPoint GetBindPoint() const override final { return VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS; }

			virtual ResourceType CreatePipelineResource() override final;

		private:
			void RecreatePipeline();

		private:
			VkPipelineInputAssemblyStateCreateInfo vIASCI = {};
			VkPipelineRasterizationStateCreateInfo vRSCI = {};
			VkPipelineViewportStateCreateInfo vVSCI = {};
			VkPipelineMultisampleStateCreateInfo vMSCI = {};

			VkPipelineColorBlendAttachmentState vCBAS = {};
			VkPipelineColorBlendStateCreateInfo vCBSCI = {};

			VkPipelineDepthStencilStateCreateInfo vDSSCI = {};
		};

		namespace _Helpers
		{
			VkPrimitiveTopology GetPrimitiveTopology(Backend::PrimitiveTopology topology);
			VkCullModeFlags GetCullMode(Backend::CullMode cull);
			VkFrontFace GetFrontFace(Backend::FrontFace face);
			VkPolygonMode GetPolygonMode(Backend::PolygonMode mode);
			VkLogicOp GetLogicOp(Backend::ColorBlendLogic logic);
			VkCompareOp GetCompareOp(Backend::DepthCompareLogic logic);
			std::vector<VkDynamicState> GetDynamicStates(Backend::DynamicStateFlags flags);
		}

		template<class TRenderTarget>
		inline void VulkanGraphicsPipeline<TRenderTarget>::Initialize(RenderTargetType* pRenderTarget, const std::vector<ShaderDigest>& shaderDigests, const Backend::GraphicsPipelineSpecification& spec)
		{
			this->pRenderTarget = pRenderTarget;
			this->mSpec = spec;
			this->mDigests = shaderDigests;
			ResolveUniformLayouts(shaderDigests);

			VulkanDevice* pDevice = pRenderTarget->GetDevice();

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
					VulkanShaderModule sModule = {};
					sModule.Initialize(pDevice, *itr);
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
			vCI.renderPass = pRenderTarget->GetRenderPass();
			vCI.basePipelineHandle = VK_NULL_HANDLE;
			vCI.basePipelineIndex = 0;
			vCI.layout = vPipelineLayout;

			VkPipeline vNewPipeline = VK_NULL_HANDLE;
			FLINT_VK_ASSERT(pDevice->CreateGraphicsPipeline(&vCI, &vNewPipeline), "Failed to create graphics pipeline!")

				if (vPipeline != VK_NULL_HANDLE)
					pRenderTarget->GetDevice()->DestroyPipeline(vPipeline);

			vPipeline = vNewPipeline;

			// Finals.
			for (auto itr = sModules.begin(); itr != sModules.end(); itr++)
				itr->Terminate();
		}

		template<class TRenderTarget>
		inline void VulkanGraphicsPipeline<TRenderTarget>::PrepareToRecreate()
		{
			VulkanDevice* pDevice = pRenderTarget->GetDevice();
			pDevice->DestroyPipeline(vPipeline);
			pDevice->DestroyPipelineLayout(vPipelineLayout);

			vPipeline = VK_NULL_HANDLE;
			vPipelineLayout = VK_NULL_HANDLE;
		}

		template<class TRenderTarget>
		inline void VulkanGraphicsPipeline<TRenderTarget>::Recreate()
		{
			RecreatePipeline();
		}

		template<class TRenderTarget>
		void VulkanGraphicsPipeline<TRenderTarget>::Terminate()
		{
			VulkanDevice* pDevice = pRenderTarget->GetDevice();
			pDevice->DestroyPipeline(vPipeline);
			pDevice->DestroyPipelineLayout(vPipelineLayout);
			pDevice->DestroyDescriptorSetLayout(vSetLayout);
		}

		template<class TRenderTarget>
		inline typename VulkanGraphicsPipeline<TRenderTarget>::ResourceType VulkanGraphicsPipeline<TRenderTarget>::CreatePipelineResource()
		{
			VulkanPipelineResource<VulkanGraphicsPipeline<TRenderTarget>> resource;
			resource.Initialize(this);

			return resource;
		}

		template<class TRenderTarget>
		inline void VulkanGraphicsPipeline<TRenderTarget>::RecreatePipeline()
		{
			VulkanDevice* pDevice = pRenderTarget->GetDevice();

			std::vector<VkPipelineShaderStageCreateInfo> vStages = {};
			std::vector<VkVertexInputAttributeDescription> vAttributeDesc = {};
			std::vector<VulkanShaderModule> sModules = {};
			VkVertexInputBindingDescription vDesc = {};

			for (auto itr = mDigests.begin(); itr != mDigests.end(); itr++)
			{
				VulkanShaderModule sModule = {};
				sModule.Initialize(pDevice, *itr);
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
			vCI.renderPass = pRenderTarget->Derive<VulkanRenderTarget>()->GetRenderPass();
			vCI.basePipelineHandle = vPipeline;
			vCI.basePipelineIndex = 0;
			vCI.layout = vPipelineLayout;

			VkPipeline vNewPipeline = VK_NULL_HANDLE;
			FLINT_VK_ASSERT(pDevice->CreateGraphicsPipeline(&vCI, &vNewPipeline), "Failed to create graphics pipeline!")

				if (vPipeline != VK_NULL_HANDLE) pDevice->DestroyPipeline(vPipeline);
			vPipeline = vNewPipeline;

			// Finals.
			for (auto itr = sModules.begin(); itr != sModules.end(); itr++)
				itr->Terminate();
		}
	}
}
