// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanCommandBuffer.hpp"
#include "VulkanBackend/VulkanCommandBufferAllocator.hpp"
#include "VulkanBackend/VulkanScreenBoundRenderTarget.hpp"
#include "VulkanBackend/VulkanOffScreenRenderTarget.hpp"
#include "VulkanBackend/VulkanGraphicsPipeline.hpp"
#include "VulkanBackend/VulkanComputePipeline.hpp"
#include "VulkanBackend/VulkanBuffer.hpp"
#include "VulkanBackend/VulkanImage.hpp"
#include "VulkanBackend/VulkanUtilities.hpp"
#include "VulkanBackend/VulkanResourcePackage.hpp"
#include "VulkanBackend/VulkanQuery.hpp"

#include "GraphicsCore/GeometryStore.hpp"
#include "GraphicsCore/DynamicStateContainer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanCommandBuffer::VulkanCommandBuffer(CommandBufferAllocator* pAllocator, VkCommandBuffer vCommandBuffer)
			: CommandBuffer(pAllocator), vCommandBuffer(vCommandBuffer)
		{
			OPTICK_EVENT();

			vSubmitInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO;
			vSubmitInfo.pNext = VK_NULL_HANDLE;
			vSubmitInfo.commandBufferCount = 1;
			vSubmitInfo.pCommandBuffers = &this->vCommandBuffer;
			vSubmitInfo.pWaitDstStageMask = &vWaitStage;
		}

		void VulkanCommandBuffer::BeginBufferRecording()
		{
			OPTICK_EVENT();

			VkCommandBufferBeginInfo vBeginInfo = {};
			vBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			vBeginInfo.pNext = VK_NULL_HANDLE;
			vBeginInfo.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			vBeginInfo.pInheritanceInfo = VK_NULL_HANDLE;

			VulkanDevice& vDevice = pAllocator->GetDevice()->StaticCast<VulkanDevice>();
			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkBeginCommandBuffer(vCommandBuffer, &vBeginInfo));

			vInFlightSemaphores.clear();
			vRenderFinishedSemaphores.clear();

			bIsRecording = true;
		}

		void VulkanCommandBuffer::BeginBufferRecording(const ScreenBoundRenderTarget* pRenderTarget)
		{
			OPTICK_EVENT();

			VkCommandBufferBeginInfo vBeginInfo = {};
			vBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			vBeginInfo.pNext = VK_NULL_HANDLE;
			vBeginInfo.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			vBeginInfo.pInheritanceInfo = pRenderTarget->StaticCast<VulkanScreenBoundRenderTarget>().GetVulkanInheritanceInfo();

			VulkanDevice& vDevice = pAllocator->GetDevice()->StaticCast<VulkanDevice>();
			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkBeginCommandBuffer(vCommandBuffer, &vBeginInfo));

			bIsRecording = true;
		}

		void VulkanCommandBuffer::BeginBufferRecording(const OffScreenRenderTarget* pRenderTarget)
		{
			OPTICK_EVENT();

			VkCommandBufferBeginInfo vBeginInfo = {};
			vBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			vBeginInfo.pNext = VK_NULL_HANDLE;
			vBeginInfo.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			vBeginInfo.pInheritanceInfo = pRenderTarget->StaticCast<VulkanOffScreenRenderTarget>().GetVulkanInheritanceInfo();

			VulkanDevice& vDevice = pAllocator->GetDevice()->StaticCast<VulkanDevice>();
			FLINT_VK_ASSERT(vDevice.GetDeviceTable().vkBeginCommandBuffer(vCommandBuffer, &vBeginInfo));

			bIsRecording = true;
		}

		void VulkanCommandBuffer::BindRenderTarget(const ScreenBoundRenderTarget* pRenderTarget)
		{
			OPTICK_EVENT();

			VulkanScreenBoundRenderTarget const& vRenderTarget = pRenderTarget->StaticCast<VulkanScreenBoundRenderTarget>();

			auto const& vSwapChain = std::static_pointer_cast<VulkanSwapChain>(vRenderTarget.GetSwapChain());
			vInFlightSemaphores.emplace_back(vSwapChain->GetInFlightSemaphore());
			vRenderFinishedSemaphores.emplace_back(vSwapChain->GetRenderFinishedSemaphore());

			VkRenderPassBeginInfo vBeginInfo = {};
			vBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			vBeginInfo.pNext = VK_NULL_HANDLE;
			vBeginInfo.renderPass = vRenderTarget.GetRenderPass();
			vBeginInfo.framebuffer = vRenderTarget.GetFramebuffer();
			vBeginInfo.clearValueCount = vRenderTarget.GetClearScreenValueCount();
			vBeginInfo.pClearValues = vRenderTarget.GetClearScreenValues();
			vBeginInfo.renderArea.extent.width = vRenderTarget.GetExtent().mWidth;
			vBeginInfo.renderArea.extent.height = vRenderTarget.GetExtent().mHeight;

			pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetDeviceTable().vkCmdBeginRenderPass(vCommandBuffer, &vBeginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
		}

		void VulkanCommandBuffer::BindRenderTargetSecondary(const ScreenBoundRenderTarget* pRenderTarget)
		{
			OPTICK_EVENT();

			VulkanScreenBoundRenderTarget const& vRenderTarget = pRenderTarget->StaticCast<VulkanScreenBoundRenderTarget>();

			auto const& vSwapChain = std::static_pointer_cast<VulkanSwapChain>(vRenderTarget.GetSwapChain());
			vInFlightSemaphores.emplace_back(vSwapChain->GetInFlightSemaphore());
			vRenderFinishedSemaphores.emplace_back(vSwapChain->GetRenderFinishedSemaphore());

			VkRenderPassBeginInfo vBeginInfo = {};
			vBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			vBeginInfo.pNext = VK_NULL_HANDLE;
			vBeginInfo.renderPass = vRenderTarget.GetRenderPass();
			vBeginInfo.framebuffer = vRenderTarget.GetFramebuffer();
			vBeginInfo.clearValueCount = vRenderTarget.GetClearScreenValueCount();
			vBeginInfo.pClearValues = vRenderTarget.GetClearScreenValues();
			vBeginInfo.renderArea.extent.width = vRenderTarget.GetExtent().mWidth;
			vBeginInfo.renderArea.extent.height = vRenderTarget.GetExtent().mHeight;

			pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetDeviceTable().vkCmdBeginRenderPass(vCommandBuffer, &vBeginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
		}

		void VulkanCommandBuffer::BindRenderTarget(const OffScreenRenderTarget* pRenderTarget)
		{
			OPTICK_EVENT();

			VulkanOffScreenRenderTarget const& vRenderTarget = pRenderTarget->StaticCast<VulkanOffScreenRenderTarget>();

			VkRenderPassBeginInfo vBeginInfo = {};
			vBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			vBeginInfo.pNext = VK_NULL_HANDLE;
			vBeginInfo.renderPass = vRenderTarget.GetRenderPass();
			vBeginInfo.framebuffer = vRenderTarget.GetFramebuffer();
			vBeginInfo.clearValueCount = vRenderTarget.GetClearScreenValueCount();
			vBeginInfo.pClearValues = vRenderTarget.GetClearScreenValues();
			vBeginInfo.renderArea.extent.width = vRenderTarget.GetExtent().mWidth;
			vBeginInfo.renderArea.extent.height = vRenderTarget.GetExtent().mHeight;

			pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetDeviceTable().vkCmdBeginRenderPass(vCommandBuffer, &vBeginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
		}

		void VulkanCommandBuffer::BindRenderTargetSecondary(const OffScreenRenderTarget* pRenderTarget)
		{
			OPTICK_EVENT();

			VulkanOffScreenRenderTarget const& vRenderTarget = pRenderTarget->StaticCast<VulkanOffScreenRenderTarget>();

			VkRenderPassBeginInfo vBeginInfo = {};
			vBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			vBeginInfo.pNext = VK_NULL_HANDLE;
			vBeginInfo.renderPass = vRenderTarget.GetRenderPass();
			vBeginInfo.framebuffer = vRenderTarget.GetFramebuffer();
			vBeginInfo.clearValueCount = vRenderTarget.GetClearScreenValueCount();
			vBeginInfo.pClearValues = vRenderTarget.GetClearScreenValues();
			vBeginInfo.renderArea.extent.width = vRenderTarget.GetExtent().mWidth;
			vBeginInfo.renderArea.extent.height = vRenderTarget.GetExtent().mHeight;

			pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetDeviceTable().vkCmdBeginRenderPass(vCommandBuffer, &vBeginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
		}

		void VulkanCommandBuffer::ClearRenderTarget(const ScreenBoundRenderTarget* pRenderTarget)
		{
			OPTICK_EVENT();

			auto& vRenderTarget = pRenderTarget->StaticCast<VulkanScreenBoundRenderTarget>();
			std::vector<VkClearAttachment> vClearAttachments(vRenderTarget.GetClearScreenValueCount());
			const auto vClearColors = vRenderTarget.GetClearScreenValueVector();
			const auto vClearAspects = vRenderTarget.GetClearAspectFlags();

			VkClearAttachment vAttachment = {};
			for (uint64_t i = 0; i < vClearAttachments.size(); i++)
			{
				vAttachment.aspectMask = vClearAspects[i];
				vAttachment.clearValue = vClearColors[i];

				if (vAttachment.aspectMask == VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT)
				{
					vClearAttachments[i] = vAttachment;
					vAttachment.colorAttachment++;
				}
				else
				{
					uint32_t temp = vAttachment.colorAttachment;
					vAttachment.colorAttachment = 0;
					vClearAttachments[i] = vAttachment;
					vAttachment.colorAttachment = temp;
				}
			}

			VkClearRect vClearRect = {};
			vClearRect.layerCount = 1;
			vClearRect.rect.offset = { 0, 0 };
			vClearRect.rect.extent = { vRenderTarget.GetExtent().mWidth, vRenderTarget.GetExtent().mHeight };

			pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetDeviceTable().vkCmdClearAttachments(vCommandBuffer, static_cast<uint32_t>(vClearAttachments.size()), vClearAttachments.data(), 1, &vClearRect);
		}

		void VulkanCommandBuffer::ClearRenderTarget(const OffScreenRenderTarget* pRenderTarget)
		{
			auto& vRenderTarget = pRenderTarget->StaticCast<VulkanOffScreenRenderTarget>();
			std::vector<VkClearAttachment> vClearAttachments(vRenderTarget.GetClearScreenValueCount());
			const auto vClearColors = vRenderTarget.GetClearScreenValueVector();
			const auto vClearAspects = vRenderTarget.GetClearAspectFlags();

			VkClearAttachment vAttachment = {};
			for (uint64_t i = 0; i < vClearAttachments.size(); i++)
			{
				vAttachment.aspectMask = vClearAspects[i];
				vAttachment.clearValue = vClearColors[i];

				if (vAttachment.aspectMask == VkImageAspectFlagBits::VK_IMAGE_ASPECT_COLOR_BIT)
				{
					vClearAttachments[i] = vAttachment;
					vAttachment.colorAttachment++;
				}
				else
				{
					uint32_t temp = vAttachment.colorAttachment;
					vAttachment.colorAttachment = 0;
					vClearAttachments[i] = vAttachment;
					vAttachment.colorAttachment = temp;
				}
			}

			VkClearRect vClearRect = {};
			vClearRect.layerCount = 1;
			vClearRect.rect.offset = { 0, 0 };
			vClearRect.rect.extent = { vRenderTarget.GetExtent().mWidth, vRenderTarget.GetExtent().mHeight };

			pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetDeviceTable().vkCmdClearAttachments(vCommandBuffer, static_cast<uint32_t>(vClearAttachments.size()), vClearAttachments.data(), 1, &vClearRect);
		}

		void VulkanCommandBuffer::UnbindRenderTarget()
		{
			OPTICK_EVENT();

			pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetDeviceTable().vkCmdEndRenderPass(vCommandBuffer);
		}

		void VulkanCommandBuffer::BindGraphicsPipeline(const GraphicsPipeline* pGraphicsPipeline)
		{
			OPTICK_EVENT();

			pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetDeviceTable().vkCmdBindPipeline(vCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pGraphicsPipeline->StaticCast<VulkanGraphicsPipeline>().GetPipeline());
		}

		void VulkanCommandBuffer::BindComputePipeline(const ComputePipeline* pComputePipeline)
		{
			OPTICK_EVENT();

			pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetDeviceTable().vkCmdBindPipeline(vCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_COMPUTE, pComputePipeline->StaticCast<VulkanComputePipeline>().GetPipeline());
		}

		void VulkanCommandBuffer::BindVertexBuffer(const Buffer* pBuffer, const uint64_t firstBinding, const uint64_t offset)
		{
			VkDeviceSize offsets[1] = { offset };
			pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetDeviceTable().vkCmdBindVertexBuffers(vCommandBuffer, static_cast<uint32_t>(firstBinding), 1, pBuffer->StaticCast<VulkanBuffer>().GetBufferAddress(), offsets);
		}

		void VulkanCommandBuffer::BindIndexBuffer(const Buffer* pBuffer, const uint64_t indexSize, const uint64_t offset)
		{
			VkIndexType indexType = VkIndexType::VK_INDEX_TYPE_UINT32;
			if (indexSize == sizeof(uint8_t))
				indexType = VkIndexType::VK_INDEX_TYPE_UINT8_EXT;

			else if (indexSize == sizeof(uint16_t))
				indexType = VkIndexType::VK_INDEX_TYPE_UINT16;

			else if (indexSize == sizeof(uint32_t))
				indexType = VkIndexType::VK_INDEX_TYPE_UINT32;

			else
				throw std::invalid_argument("Invalid index size submitted to bind! The valid sizes are 1, 2, and 4 bytes.");

			pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetDeviceTable().vkCmdBindIndexBuffer(vCommandBuffer, pBuffer->StaticCast<VulkanBuffer>().GetBuffer(), offset, indexType);
		}

		void VulkanCommandBuffer::BindGeometryStore(const GeometryStore* pGeometryStore)
		{
			OPTICK_EVENT();

			BindVertexBuffer(pGeometryStore->GetVertexBuffer());
			BindIndexBuffer(pGeometryStore->GetIndexBuffer(), pGeometryStore->GetIndexSize());
		}

		void VulkanCommandBuffer::BindResourcePackage(const GraphicsPipeline* pPipeline, ResourcePackage* pResourcePackage)
		{
			auto& vPackage = pResourcePackage->StaticCast<VulkanResourcePackage>();
			vPackage.PrepareIfNecessary();

			pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetDeviceTable().vkCmdBindDescriptorSets(vCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->StaticCast<VulkanGraphicsPipeline>().GetPipelineLayout(), 0, 1, vPackage.GetDescriptorSetAddress(), 0, nullptr);
		}

		void VulkanCommandBuffer::BindResourcePackages(const GraphicsPipeline* pPipeline, const std::vector<ResourcePackage*>& pResourcePackages)
		{
			if (pResourcePackages.empty())
				return;

			std::vector<VkDescriptorSet> vDescriptorSets(pResourcePackages.size());
			for (uint64_t i = 0; i < pResourcePackages.size(); i++)
			{
				auto& vPackage = pResourcePackages[i]->StaticCast<VulkanResourcePackage>();
				vPackage.PrepareIfNecessary();
				vDescriptorSets[i] = vPackage.GetDescriptorSet();
			}

			pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetDeviceTable().vkCmdBindDescriptorSets(vCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->StaticCast<VulkanGraphicsPipeline>().GetPipelineLayout(), 0, static_cast<uint32_t>(vDescriptorSets.size()), vDescriptorSets.data(), 0, nullptr);
		}

		void VulkanCommandBuffer::BindResourcePackage(const ComputePipeline* pPipeline, ResourcePackage* pResourcePackage)
		{
			auto& vPackage = pResourcePackage->StaticCast<VulkanResourcePackage>();
			vPackage.PrepareIfNecessary();

			pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetDeviceTable().vkCmdBindDescriptorSets(vCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_COMPUTE, pPipeline->StaticCast<VulkanComputePipeline>().GetPipelineLayout(), 0, 1, vPackage.GetDescriptorSetAddress(), 0, nullptr);
		}

		void VulkanCommandBuffer::BindResourcePackages(const ComputePipeline* pPipeline, const std::vector<ResourcePackage*>& pResourcePackages)
		{
			if (pResourcePackages.empty())
				return;

			std::vector<VkDescriptorSet> vDescriptorSets(pResourcePackages.size());
			for (uint64_t i = 0; i < pResourcePackages.size(); i++)
			{
				auto& vPackage = pResourcePackages[i]->StaticCast<VulkanResourcePackage>();
				vPackage.PrepareIfNecessary();
				vDescriptorSets[i] = vPackage.GetDescriptorSet();
			}

			pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetDeviceTable().vkCmdBindDescriptorSets(vCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_COMPUTE, pPipeline->StaticCast<VulkanComputePipeline>().GetPipelineLayout(), 0, static_cast<uint32_t>(vDescriptorSets.size()), vDescriptorSets.data(), 0, nullptr);
		}

		void VulkanCommandBuffer::BindDynamicStates(const GraphicsPipeline* pPipeline, const DynamicStateContainer* pDynamicStates)
		{
			OPTICK_EVENT();

			if (!pDynamicStates)
				return;

			if (pDynamicStates->mFlags & DynamicStateFlags::ViewPort)
				BindViewPort(pPipeline, &pDynamicStates->mViewPort);

			if (pDynamicStates->mFlags & DynamicStateFlags::Scissor)
				BindScissor(pPipeline, &pDynamicStates->mScissor);

			if (pDynamicStates->mFlags & DynamicStateFlags::LineWidth)
				BindLineWidth(pPipeline, &pDynamicStates->mLineWidth);

			if (pDynamicStates->mFlags & DynamicStateFlags::DepthBias)
				BindDepthBias(pPipeline, &pDynamicStates->mDepthBias);

			if (pDynamicStates->mFlags & DynamicStateFlags::BlendConstants)
				BindBlendConstants(pPipeline, &pDynamicStates->mBlendConstants);

			if (pDynamicStates->mFlags & DynamicStateFlags::DepthBounds)
				BindDepthBounds(pPipeline, &pDynamicStates->mDepthBounds);

			for (uint8_t i = 0; i < 10; i++)
				if (!pDynamicStates->mConstantBlocks[i].IsNull())
					BindConstantData(pPipeline, &pDynamicStates->mConstantBlocks[i], ShaderType(i + 1));
		}

		void VulkanCommandBuffer::BindViewPort(const GraphicsPipeline* pPipeline, const ViewPort* pViewPort)
		{
			OPTICK_EVENT();

			VkViewport vVP = {};
			vVP.width = pViewPort->mExtent.mWidth;
			vVP.height = pViewPort->mExtent.mHeight;
			vVP.minDepth = pViewPort->mDepth.mWidth;
			vVP.maxDepth = pViewPort->mDepth.mHeight;
			vVP.x = pViewPort->mOffset.mWidth;
			vVP.y = pViewPort->mOffset.mHeight;

			VulkanDevice& vDevice = pAllocator->GetDevice()->StaticCast<VulkanDevice>();
			vDevice.GetDeviceTable().vkCmdSetViewport(vCommandBuffer, 0, 1, &vVP);
		}

		void VulkanCommandBuffer::BindScissor(const GraphicsPipeline* pPipeline, const Scissor* pScissor)
		{
			OPTICK_EVENT();

			VkRect2D vR2D = {};
			vR2D.extent.width = pScissor->mExtent.mWidth;
			vR2D.extent.height = pScissor->mExtent.mHeight;
			vR2D.offset.x = pScissor->mOffset.mWidth;
			vR2D.offset.y = pScissor->mOffset.mHeight;

			VulkanDevice& vDevice = pAllocator->GetDevice()->StaticCast<VulkanDevice>();
			vDevice.GetDeviceTable().vkCmdSetScissor(vCommandBuffer, 0, 1, &vR2D);
		}

		void VulkanCommandBuffer::BindLineWidth(const GraphicsPipeline* pPipeline, const LineWidth* pLineWidth)
		{
			OPTICK_EVENT();

			VulkanDevice& vDevice = pAllocator->GetDevice()->StaticCast<VulkanDevice>();
			vDevice.GetDeviceTable().vkCmdSetLineWidth(vCommandBuffer, pLineWidth->mLineWidth);
		}

		void VulkanCommandBuffer::BindDepthBias(const GraphicsPipeline* pPipeline, const DepthBias* pDepthBias)
		{
			OPTICK_EVENT();

			VulkanDevice& vDevice = pAllocator->GetDevice()->StaticCast<VulkanDevice>();
			vDevice.GetDeviceTable().vkCmdSetDepthBias(vCommandBuffer, pDepthBias->mDepthBiasFactor, pDepthBias->mDepthClampFactor, pDepthBias->mDepthSlopeFactor);
		}

		void VulkanCommandBuffer::BindBlendConstants(const GraphicsPipeline* pPipeline, const BlendConstants* pBlendConstants)
		{
			OPTICK_EVENT();

			VulkanDevice& vDevice = pAllocator->GetDevice()->StaticCast<VulkanDevice>();
			vDevice.GetDeviceTable().vkCmdSetBlendConstants(vCommandBuffer, pBlendConstants->mConstants);
		}

		void VulkanCommandBuffer::BindDepthBounds(const GraphicsPipeline* pPipeline, const DepthBounds* pDepthBounds)
		{
			OPTICK_EVENT();

			VulkanDevice& vDevice = pAllocator->GetDevice()->StaticCast<VulkanDevice>();
			vDevice.GetDeviceTable().vkCmdSetDepthBounds(vCommandBuffer, pDepthBounds->mBounds.mWidth, pDepthBounds->mBounds.mHeight);
		}

		void VulkanCommandBuffer::BindConstantData(const GraphicsPipeline* pPipeline, const ConstantData* pConstantData, const ShaderType type)
		{
			OPTICK_EVENT();

			VulkanDevice& vDevice = pAllocator->GetDevice()->StaticCast<VulkanDevice>();
			vDevice.GetDeviceTable().vkCmdPushConstants(vCommandBuffer, pPipeline->StaticCast<VulkanGraphicsPipeline>().GetPipelineLayout(), Utilities::GetShaderStage(type), static_cast<uint32_t>(pConstantData->mOffset), static_cast<uint32_t>(pConstantData->mSize), pConstantData->pData);
		}

		void VulkanCommandBuffer::BindDynamicStates(const ComputePipeline* pPipeline, const DynamicStateContainer* pDynamicStates)
		{
			OPTICK_EVENT();

			if (!pDynamicStates)
				return;

			// Just bind the constant data as all the other dynamic states are not supported.
			BindConstantData(pPipeline, &pDynamicStates->mConstantBlocks[5]);
		}

		void VulkanCommandBuffer::BindConstantData(const ComputePipeline* pPipeline, const ConstantData* pConstantData)
		{
			OPTICK_EVENT();

			VulkanDevice& vDevice = pAllocator->GetDevice()->StaticCast<VulkanDevice>();
			vDevice.GetDeviceTable().vkCmdPushConstants(vCommandBuffer, pPipeline->StaticCast<VulkanComputePipeline>().GetPipelineLayout(), VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT, static_cast<uint32_t>(pConstantData->mOffset), static_cast<uint32_t>(pConstantData->mSize), pConstantData->pData);
		}

		void VulkanCommandBuffer::IssueDrawCall(WireFrame& wireFrame, const uint64_t firstInstance, const uint64_t instanceCount, const DrawCallMode mode)
		{
			OPTICK_EVENT();

			VulkanDevice& vDevice = pAllocator->GetDevice()->StaticCast<VulkanDevice>();
			if (mode == DrawCallMode::Indexed)
				vDevice.GetDeviceTable().vkCmdDrawIndexed(vCommandBuffer, static_cast<uint32_t>(wireFrame.GetIndexCount()), static_cast<uint32_t>(instanceCount), static_cast<uint32_t>(wireFrame.GetIndexOffset()), static_cast<uint32_t>(wireFrame.GetVertexOffset()), static_cast<uint32_t>(firstInstance));

			else if (mode == DrawCallMode::Vertex)
				vDevice.GetDeviceTable().vkCmdDraw(vCommandBuffer, static_cast<uint32_t>(wireFrame.GetVertexCount()), static_cast<uint32_t>(instanceCount), static_cast<uint32_t>(wireFrame.GetVertexOffset()), static_cast<uint32_t>(firstInstance));

			else
				throw backend_error("Invalid draw call mode!");
		}

		void VulkanCommandBuffer::IssueDrawCall(WireFrame&& wireFrame, const uint64_t firstInstance, const uint64_t instanceCount, const DrawCallMode mode)
		{
			OPTICK_EVENT();

			VulkanDevice& vDevice = pAllocator->GetDevice()->StaticCast<VulkanDevice>();
			if (mode == DrawCallMode::Indexed)
				vDevice.GetDeviceTable().vkCmdDrawIndexed(vCommandBuffer, static_cast<uint32_t>(wireFrame.GetIndexCount()), static_cast<uint32_t>(instanceCount), static_cast<uint32_t>(wireFrame.GetIndexOffset()), static_cast<uint32_t>(wireFrame.GetVertexOffset()), static_cast<uint32_t>(firstInstance));

			else if (mode == DrawCallMode::Vertex)
				vDevice.GetDeviceTable().vkCmdDraw(vCommandBuffer, static_cast<uint32_t>(wireFrame.GetVertexCount()), static_cast<uint32_t>(instanceCount), static_cast<uint32_t>(wireFrame.GetVertexOffset()), static_cast<uint32_t>(firstInstance));

			else
				throw backend_error("Invalid draw call mode!");
		}

		void VulkanCommandBuffer::IssueComputeCall(const FBox3D& groups)
		{
			OPTICK_EVENT();

			pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetDeviceTable().vkCmdDispatch(vCommandBuffer, groups.X, groups.Y, groups.Z);
		}

		void VulkanCommandBuffer::CopyImage(const Image* pSourceImage, const FBox3D sourceOffset, Image* pDestinationImage, const FBox3D destinationOffset)
		{
			OPTICK_EVENT();

			// Return if either one of images are null.
			if (!pSourceImage || !pDestinationImage)
				throw backend_error("One or more of the submitted images are null!");

			auto const& vSourceImage = pSourceImage->StaticCast<VulkanImage>();
			auto& vDestinationImage = pDestinationImage->StaticCast<VulkanImage>();

			VkOffset3D vSourceOffset = {};
			vSourceOffset.x = sourceOffset.X;
			vSourceOffset.y = sourceOffset.Y;
			vSourceOffset.z = sourceOffset.Z;

			VkOffset3D vDestinationOffset = {};
			vDestinationOffset.x = destinationOffset.X;
			vDestinationOffset.y = destinationOffset.Y;
			vDestinationOffset.z = destinationOffset.Z;

			VkImageSubresourceLayers vLayers = {};
			vLayers.baseArrayLayer = 0;
			vLayers.layerCount = vSourceImage.GetLayerCount();
			vLayers.mipLevel = 0; // TODO
			vLayers.aspectMask = vSourceImage.GetAspectFlags();

			const auto vOldDstLayout = vDestinationImage.GetImageLayout();
			const auto vOldSrcLayout = vSourceImage.GetImageLayout();

			vDestinationImage.SetImageLayoutManual(vCommandBuffer, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			vSourceImage.SetImageLayoutManual(vCommandBuffer, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

			vDestinationImage.CopyFromImage(vCommandBuffer, vSourceImage.GetImage(), vSourceImage.GetImageLayout(), vSourceOffset, vDestinationOffset, vLayers);

			vDestinationImage.SetImageLayoutManual(vCommandBuffer, vOldDstLayout);
			vSourceImage.SetImageLayoutManual(vCommandBuffer, vOldSrcLayout);
		}

		void VulkanCommandBuffer::CopyToSwapChainImage(const Image* pSourceImage, const FBox3D sourceOffset, SwapChain* pSwapChain, const uint32_t imageIndex, const FBox3D destinationOffset)
		{
			OPTICK_EVENT();

			const auto& vSourceImage = pSourceImage->StaticCast<VulkanImage>();
			auto& vSwapChain = pSwapChain->StaticCast<VulkanSwapChain>();

			VkOffset3D vSourceOffset = {};
			vSourceOffset.x = sourceOffset.X;
			vSourceOffset.y = sourceOffset.Y;
			vSourceOffset.z = sourceOffset.Z;

			VkOffset3D vDestinationOffset = {};
			vDestinationOffset.x = destinationOffset.X;
			vDestinationOffset.y = destinationOffset.Y;
			vDestinationOffset.z = destinationOffset.Z;

			VkImageSubresourceLayers vLayers = {};
			vLayers.baseArrayLayer = 0;
			vLayers.layerCount = vSourceImage.GetLayerCount();
			vLayers.mipLevel = 0;
			//vLayers.mipLevel = vSourceImage.GetMipLevels();
			vLayers.aspectMask = vSourceImage.GetAspectFlags();

			const auto vOldSrcLayout = vSourceImage.GetImageLayout();

			vSourceImage.SetImageLayoutManual(vCommandBuffer, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
			vSwapChain.CopyFromImage(vCommandBuffer, vSourceImage.GetImage(), vSourceImage.GetImageLayout(), vSourceOffset, vDestinationOffset, imageIndex, vLayers);
			vSourceImage.SetImageLayoutManual(vCommandBuffer, vOldSrcLayout);

			VkMemoryBarrier vMemoryBarrier = {};
			VkBufferMemoryBarrier vBufferMemoryBarrier = {};
			VkImageMemoryBarrier vImageMemoryBarrier = {};
		}

		void VulkanCommandBuffer::SubmitSecondaryCommandBuffer(const std::shared_ptr<CommandBuffer>& pCommandBuffer)
		{
			vSecondaryCommandBuffers.emplace_back(pCommandBuffer->StaticCast<VulkanCommandBuffer>().GetVulkanCommandBuffer());
		}

		void VulkanCommandBuffer::ExecuteSecondaryCommands()
		{
			OPTICK_EVENT();

			if (!vSecondaryCommandBuffers.empty())
			{
				pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetDeviceTable().vkCmdExecuteCommands(vCommandBuffer, static_cast<uint32_t>(vSecondaryCommandBuffers.size()), vSecondaryCommandBuffers.data());
				vSecondaryCommandBuffers.clear();
			}
		}

		void VulkanCommandBuffer::EndBufferRecording()
		{
			OPTICK_EVENT();

			FLINT_VK_ASSERT(pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetDeviceTable().vkEndCommandBuffer(vCommandBuffer));
			bIsRecording = false;
		}

		void VulkanCommandBuffer::IncludeSwapChain(SwapChain* pSwapChain)
		{
			auto const& vSwapChain = pSwapChain->StaticCast<VulkanSwapChain>();
			vInFlightSemaphores.emplace_back(vSwapChain.GetInFlightSemaphore());
			vRenderFinishedSemaphores.emplace_back(vSwapChain.GetRenderFinishedSemaphore());
		}

		void VulkanCommandBuffer::BeginQuery(const Query* pQuery, const uint32_t index, const bool requirePrecision)
		{
			OPTICK_EVENT();

			auto const& vQuery = pQuery->StaticCast<VulkanQuery>();
			pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetDeviceTable().vkCmdBeginQuery(vCommandBuffer, vQuery.GetQuery(), index, requirePrecision ? VkQueryControlFlagBits::VK_QUERY_CONTROL_PRECISE_BIT : 0);
		}

		void VulkanCommandBuffer::EndQuery(const Query* pQuery, const uint32_t index)
		{
			OPTICK_EVENT();

			auto const& vQuery = pQuery->StaticCast<VulkanQuery>();
			pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetDeviceTable().vkCmdEndQuery(vCommandBuffer, vQuery.GetQuery(), index);
		}

		void VulkanCommandBuffer::ResetQuery(const Query* pQuery, const uint32_t beginIndex, const uint32_t count)
		{
			OPTICK_EVENT();

			auto const& vQuery = pQuery->StaticCast<VulkanQuery>();
			pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetDeviceTable().vkCmdResetQueryPool(vCommandBuffer, vQuery.GetQuery(), beginIndex, count);
		}

		void VulkanCommandBuffer::Synchronize()
		{
			OPTICK_EVENT();

			VkMemoryBarrier vMemoryBarrier = {};
			VkBufferMemoryBarrier vBufferMemoryBarrier = {};
			VkImageMemoryBarrier vImageMemoryBarrier = {};

			auto& vDevice = pAllocator->GetDevice()->StaticCast<VulkanDevice>();
			vDevice.GetDeviceTable().vkCmdPipelineBarrier(
				vCommandBuffer,
				VkPipelineStageFlagBits(),
				VkPipelineStageFlagBits(),
				VkDependencyFlagBits(),
				1,
				&vMemoryBarrier,
				1,
				&vBufferMemoryBarrier,
				1,
				&vImageMemoryBarrier
			);
		}

		void VulkanCommandBuffer::Terminate()
		{
			bIsTerminated = true;
		}

		const VkSubmitInfo VulkanCommandBuffer::GetSubmitInfo() const
		{
			vSubmitInfo.signalSemaphoreCount = static_cast<uint32_t>(vRenderFinishedSemaphores.size());
			vSubmitInfo.pSignalSemaphores = vRenderFinishedSemaphores.data();
			vSubmitInfo.waitSemaphoreCount = static_cast<uint32_t>(vInFlightSemaphores.size());
			vSubmitInfo.pWaitSemaphores = vInFlightSemaphores.data();

			return vSubmitInfo;
		}

		const VkSubmitInfo* VulkanCommandBuffer::GetSubmitInfoAddress() const
		{
			vSubmitInfo.signalSemaphoreCount = static_cast<uint32_t>(vRenderFinishedSemaphores.size());
			vSubmitInfo.pSignalSemaphores = vRenderFinishedSemaphores.data();
			vSubmitInfo.waitSemaphoreCount = static_cast<uint32_t>(vInFlightSemaphores.size());
			vSubmitInfo.pWaitSemaphores = vInFlightSemaphores.data();

			return &vSubmitInfo;
		}
	}
}