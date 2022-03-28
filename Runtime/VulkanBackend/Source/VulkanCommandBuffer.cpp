// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanCommandBuffer.hpp"

#include "GraphicsCore/GeometryStore.hpp"
#include "GraphicsCore/DynamicStateContainer.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanCommandBuffer::VulkanCommandBuffer(VulkanCommandBufferAllocator* pAllocator, VkCommandBuffer vCommandBuffer)
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

			FLINT_VK_ASSERT(pAllocator->GetDevice()->GetDeviceTable().vkBeginCommandBuffer(vCommandBuffer, &vBeginInfo));

			vInFlightSemaphores.clear();
			vRenderFinishedSemaphores.clear();

			bIsRecording = true;
		}

		void VulkanCommandBuffer::BeginBufferRecording(const VulkanScreenBoundRenderTarget* pRenderTarget)
		{
			OPTICK_EVENT();

			VkCommandBufferBeginInfo vBeginInfo = {};
			vBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			vBeginInfo.pNext = VK_NULL_HANDLE;
			vBeginInfo.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			vBeginInfo.pInheritanceInfo = pRenderTarget->GetVulkanInheritanceInfo();

			FLINT_VK_ASSERT(pAllocator->GetDevice()->GetDeviceTable().vkBeginCommandBuffer(vCommandBuffer, &vBeginInfo));

			bIsRecording = true;
		}

		void VulkanCommandBuffer::BeginBufferRecording(const VulkanOffScreenRenderTarget* pRenderTarget)
		{
			OPTICK_EVENT();

			VkCommandBufferBeginInfo vBeginInfo = {};
			vBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			vBeginInfo.pNext = VK_NULL_HANDLE;
			vBeginInfo.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			vBeginInfo.pInheritanceInfo = pRenderTarget->GetVulkanInheritanceInfo();

			FLINT_VK_ASSERT(pAllocator->GetDevice()->GetDeviceTable().vkBeginCommandBuffer(vCommandBuffer, &vBeginInfo));

			bIsRecording = true;
		}

		void VulkanCommandBuffer::BindRenderTarget(const VulkanScreenBoundRenderTarget* pRenderTarget)
		{
			OPTICK_EVENT();

			auto const& vSwapChain = static_cast<VulkanSwapChain*>(pRenderTarget->GetSwapChain());
			vInFlightSemaphores.emplace_back(vSwapChain->GetInFlightSemaphore());
			vRenderFinishedSemaphores.emplace_back(vSwapChain->GetRenderFinishedSemaphore());

			VkRenderPassBeginInfo vBeginInfo = {};
			vBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			vBeginInfo.pNext = VK_NULL_HANDLE;
			vBeginInfo.renderPass = pRenderTarget->GetRenderPass();
			vBeginInfo.framebuffer = pRenderTarget->GetFramebuffer();
			vBeginInfo.clearValueCount = pRenderTarget->GetClearScreenValueCount();
			vBeginInfo.pClearValues = pRenderTarget->GetClearScreenValues();
			vBeginInfo.renderArea.extent.width = pRenderTarget->GetExtent().m_Width;
			vBeginInfo.renderArea.extent.height = pRenderTarget->GetExtent().m_Height;

			pAllocator->GetDevice()->GetDeviceTable().vkCmdBeginRenderPass(vCommandBuffer, &vBeginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
		}

		void VulkanCommandBuffer::BindRenderTargetSecondary(const VulkanScreenBoundRenderTarget* pRenderTarget)
		{
			OPTICK_EVENT();

			auto const& vSwapChain = static_cast<VulkanSwapChain*>(pRenderTarget->GetSwapChain());
			vInFlightSemaphores.emplace_back(vSwapChain->GetInFlightSemaphore());
			vRenderFinishedSemaphores.emplace_back(vSwapChain->GetRenderFinishedSemaphore());

			VkRenderPassBeginInfo vBeginInfo = {};
			vBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			vBeginInfo.pNext = VK_NULL_HANDLE;
			vBeginInfo.renderPass = pRenderTarget->GetRenderPass();
			vBeginInfo.framebuffer = pRenderTarget->GetFramebuffer();
			vBeginInfo.clearValueCount = pRenderTarget->GetClearScreenValueCount();
			vBeginInfo.pClearValues = pRenderTarget->GetClearScreenValues();
			vBeginInfo.renderArea.extent.width = pRenderTarget->GetExtent().m_Width;
			vBeginInfo.renderArea.extent.height = pRenderTarget->GetExtent().m_Height;

			pAllocator->GetDevice()->GetDeviceTable().vkCmdBeginRenderPass(vCommandBuffer, &vBeginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
		}

		void VulkanCommandBuffer::BindRenderTarget(const VulkanOffScreenRenderTarget* pRenderTarget)
		{
			OPTICK_EVENT();

			VkRenderPassBeginInfo vBeginInfo = {};
			vBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			vBeginInfo.pNext = VK_NULL_HANDLE;
			vBeginInfo.renderPass = pRenderTarget->GetRenderPass();
			vBeginInfo.framebuffer = pRenderTarget->GetFramebuffer();
			vBeginInfo.clearValueCount = pRenderTarget->GetClearScreenValueCount();
			vBeginInfo.pClearValues = pRenderTarget->GetClearScreenValues();
			vBeginInfo.renderArea.extent.width = pRenderTarget->GetExtent().m_Width;
			vBeginInfo.renderArea.extent.height = pRenderTarget->GetExtent().m_Height;

			pAllocator->GetDevice()->GetDeviceTable().vkCmdBeginRenderPass(vCommandBuffer, &vBeginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
		}

		void VulkanCommandBuffer::BindRenderTargetSecondary(const VulkanOffScreenRenderTarget* pRenderTarget)
		{
			OPTICK_EVENT();

			VkRenderPassBeginInfo vBeginInfo = {};
			vBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			vBeginInfo.pNext = VK_NULL_HANDLE;
			vBeginInfo.renderPass = pRenderTarget->GetRenderPass();
			vBeginInfo.framebuffer = pRenderTarget->GetFramebuffer();
			vBeginInfo.clearValueCount = pRenderTarget->GetClearScreenValueCount();
			vBeginInfo.pClearValues = pRenderTarget->GetClearScreenValues();
			vBeginInfo.renderArea.extent.width = pRenderTarget->GetExtent().m_Width;
			vBeginInfo.renderArea.extent.height = pRenderTarget->GetExtent().m_Height;

			pAllocator->GetDevice()->GetDeviceTable().vkCmdBeginRenderPass(vCommandBuffer, &vBeginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
		}

		void VulkanCommandBuffer::ClearRenderTarget(const VulkanScreenBoundRenderTarget* pRenderTarget)
		{
			OPTICK_EVENT();

			std::vector<VkClearAttachment> vClearAttachments(pRenderTarget->GetClearScreenValueCount());
			const auto vClearColors = pRenderTarget->GetClearScreenValueVector();
			const auto vClearAspects = pRenderTarget->GetClearAspectFlags();

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
			vClearRect.rect.extent = { pRenderTarget->GetExtent().m_Width, pRenderTarget->GetExtent().m_Height };

			pAllocator->GetDevice()->GetDeviceTable().vkCmdClearAttachments(vCommandBuffer, static_cast<uint32_t>(vClearAttachments.size()), vClearAttachments.data(), 1, &vClearRect);
		}

		void VulkanCommandBuffer::ClearRenderTarget(const VulkanOffScreenRenderTarget* pRenderTarget)
		{
			std::vector<VkClearAttachment> vClearAttachments(pRenderTarget->GetClearScreenValueCount());
			const auto vClearColors = pRenderTarget->GetClearScreenValueVector();
			const auto vClearAspects = pRenderTarget->GetClearAspectFlags();

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
			vClearRect.rect.extent = { pRenderTarget->GetExtent().m_Width, pRenderTarget->GetExtent().m_Height };

			pAllocator->GetDevice()->GetDeviceTable().vkCmdClearAttachments(vCommandBuffer, static_cast<uint32_t>(vClearAttachments.size()), vClearAttachments.data(), 1, &vClearRect);
		}

		void VulkanCommandBuffer::UnbindRenderTarget()
		{
			OPTICK_EVENT();

			pAllocator->GetDevice()->GetDeviceTable().vkCmdEndRenderPass(vCommandBuffer);
		}

		void VulkanCommandBuffer::BindGraphicsPipeline(const VulkanGraphicsPipeline* pGraphicsPipeline)
		{
			OPTICK_EVENT();

			pAllocator->GetDevice()->GetDeviceTable().vkCmdBindPipeline(vCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pGraphicsPipeline->GetPipeline());
		}

		void VulkanCommandBuffer::BindComputePipeline(const VulkanComputePipeline* pComputePipeline)
		{
			OPTICK_EVENT();

			pAllocator->GetDevice()->GetDeviceTable().vkCmdBindPipeline(vCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_COMPUTE, pComputePipeline->GetPipeline());
		}

		void VulkanCommandBuffer::BindVertexBuffer(const VulkanBuffer* pBuffer, const uint64_t firstBinding, const uint64_t offset)
		{
			VkDeviceSize offsets[1] = { offset };
			pAllocator->GetDevice()->GetDeviceTable().vkCmdBindVertexBuffers(vCommandBuffer, static_cast<uint32_t>(firstBinding), 1, pBuffer->GetBufferAddress(), offsets);
		}

		void VulkanCommandBuffer::BindIndexBuffer(const VulkanBuffer* pBuffer, const uint64_t indexSize, const uint64_t offset)
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

			pAllocator->GetDevice()->GetDeviceTable().vkCmdBindIndexBuffer(vCommandBuffer, pBuffer->GetBuffer(), offset, indexType);
		}

		void VulkanCommandBuffer::BindGeometryStore(const VulkanGeometryStore* pGeometryStore)
		{
			OPTICK_EVENT();

			BindVertexBuffer(pGeometryStore->GetVertexBuffer());
			BindIndexBuffer(pGeometryStore->GetIndexBuffer(), pGeometryStore->GetIndexSize());
		}

		void VulkanCommandBuffer::BindResourcePackage(const VulkanGraphicsPipeline* pPipeline, VulkanResourcePackage* pResourcePackage)
		{
			pResourcePackage->PrepareIfNecessary();
			pAllocator->GetDevice()->GetDeviceTable().vkCmdBindDescriptorSets(vCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->GetPipelineLayout(), 0, 1, pResourcePackage->GetDescriptorSetAddress(), 0, nullptr);
		}

		void VulkanCommandBuffer::BindResourcePackages(const VulkanGraphicsPipeline* pPipeline, const std::vector<VulkanResourcePackage*>& pResourcePackages)
		{
			if (pResourcePackages.empty())
				return;

			std::vector<VkDescriptorSet> vDescriptorSets(pResourcePackages.size());
			for (uint64_t i = 0; i < pResourcePackages.size(); i++)
			{
				auto pResourcePackage = pResourcePackages[i];
				pResourcePackage->PrepareIfNecessary();
				vDescriptorSets[i] = pResourcePackage->GetDescriptorSet();
			}

			pAllocator->GetDevice()->GetDeviceTable().vkCmdBindDescriptorSets(vCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->GetPipelineLayout(), 0, static_cast<uint32_t>(vDescriptorSets.size()), vDescriptorSets.data(), 0, nullptr);
		}

		void VulkanCommandBuffer::BindResourcePackage(const VulkanComputePipeline* pPipeline, VulkanResourcePackage* pResourcePackage)
		{
			pResourcePackage->PrepareIfNecessary();
			pAllocator->GetDevice()->GetDeviceTable().vkCmdBindDescriptorSets(vCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_COMPUTE, pPipeline->GetPipelineLayout(), 0, 1, pResourcePackage->GetDescriptorSetAddress(), 0, nullptr);
		}

		void VulkanCommandBuffer::BindResourcePackages(const VulkanComputePipeline* pPipeline, const std::vector<VulkanResourcePackage*>& pResourcePackages)
		{
			if (pResourcePackages.empty())
				return;

			std::vector<VkDescriptorSet> vDescriptorSets(pResourcePackages.size());
			for (uint64_t i = 0; i < pResourcePackages.size(); i++)
			{
				auto pResourcePackage = pResourcePackages[i];
				pResourcePackage->PrepareIfNecessary();
				vDescriptorSets[i] = pResourcePackage->GetDescriptorSet();
			}

			pAllocator->GetDevice()->GetDeviceTable().vkCmdBindDescriptorSets(vCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_COMPUTE, pPipeline->GetPipelineLayout(), 0, static_cast<uint32_t>(vDescriptorSets.size()), vDescriptorSets.data(), 0, nullptr);
		}

		void VulkanCommandBuffer::BindDynamicStates(const VulkanGraphicsPipeline* pPipeline, const DynamicStateContainer* pDynamicStates)
		{
			OPTICK_EVENT();

			if (!pDynamicStates)
				return;

			if (pDynamicStates->m_Flags & DynamicStateFlags::ViewPort)
				BindViewPort(pPipeline, &pDynamicStates->m_ViewPort);

			if (pDynamicStates->m_Flags & DynamicStateFlags::Scissor)
				BindScissor(pPipeline, &pDynamicStates->m_Scissor);

			if (pDynamicStates->m_Flags & DynamicStateFlags::LineWidth)
				BindLineWidth(pPipeline, &pDynamicStates->m_LineWidth);

			if (pDynamicStates->m_Flags & DynamicStateFlags::DepthBias)
				BindDepthBias(pPipeline, &pDynamicStates->m_DepthBias);

			if (pDynamicStates->m_Flags & DynamicStateFlags::BlendConstants)
				BindBlendConstants(pPipeline, &pDynamicStates->m_BlendConstants);

			if (pDynamicStates->m_Flags & DynamicStateFlags::DepthBounds)
				BindDepthBounds(pPipeline, &pDynamicStates->m_DepthBounds);

			for (uint8_t i = 0; i < 10; i++)
			{
				if (!pDynamicStates->m_ConstantBlocks[i].IsNull())
					BindConstantData(pPipeline, &pDynamicStates->m_ConstantBlocks[i], ShaderType(i + 1));
			}
		}

		void VulkanCommandBuffer::BindViewPort(const VulkanGraphicsPipeline* pPipeline, const ViewPort* pViewPort)
		{
			OPTICK_EVENT();

			VkViewport vVP = {};
			vVP.width = pViewPort->m_Extent.m_Width;
			vVP.height = pViewPort->m_Extent.m_Height;
			vVP.minDepth = pViewPort->m_Depth.m_Width;
			vVP.maxDepth = pViewPort->m_Depth.m_Height;
			vVP.x = pViewPort->m_Offset.m_Width;
			vVP.y = pViewPort->m_Offset.m_Height;

			pAllocator->GetDevice()->GetDeviceTable().vkCmdSetViewport(vCommandBuffer, 0, 1, &vVP);
		}

		void VulkanCommandBuffer::BindScissor(const VulkanGraphicsPipeline* pPipeline, const Scissor* pScissor)
		{
			OPTICK_EVENT();

			VkRect2D vR2D = {};
			vR2D.extent.width = pScissor->m_Extent.m_Width;
			vR2D.extent.height = pScissor->m_Extent.m_Height;
			vR2D.offset.x = pScissor->m_Offset.m_Width;
			vR2D.offset.y = pScissor->m_Offset.m_Height;

			pAllocator->GetDevice()->GetDeviceTable().vkCmdSetScissor(vCommandBuffer, 0, 1, &vR2D);
		}

		void VulkanCommandBuffer::BindLineWidth(const VulkanGraphicsPipeline* pPipeline, const LineWidth* pLineWidth)
		{
			OPTICK_EVENT();

			pAllocator->GetDevice()->GetDeviceTable().vkCmdSetLineWidth(vCommandBuffer, pLineWidth->m_LineWidth);
		}

		void VulkanCommandBuffer::BindDepthBias(const VulkanGraphicsPipeline* pPipeline, const DepthBias* pDepthBias)
		{
			OPTICK_EVENT();

			pAllocator->GetDevice()->GetDeviceTable().vkCmdSetDepthBias(vCommandBuffer, pDepthBias->m_DepthBiasFactor, pDepthBias->m_DepthClampFactor, pDepthBias->m_DepthSlopeFactor);
		}

		void VulkanCommandBuffer::BindBlendConstants(const VulkanGraphicsPipeline* pPipeline, const BlendConstants* pBlendConstants)
		{
			OPTICK_EVENT();

			pAllocator->GetDevice()->GetDeviceTable().vkCmdSetBlendConstants(vCommandBuffer, pBlendConstants->m_Constants);
		}

		void VulkanCommandBuffer::BindDepthBounds(const VulkanGraphicsPipeline* pPipeline, const DepthBounds* pDepthBounds)
		{
			OPTICK_EVENT();

			pAllocator->GetDevice()->GetDeviceTable().vkCmdSetDepthBounds(vCommandBuffer, pDepthBounds->m_Bounds.m_Width, pDepthBounds->m_Bounds.m_Height);
		}

		void VulkanCommandBuffer::BindConstantData(const VulkanGraphicsPipeline* pPipeline, const ConstantData* pConstantData, const ShaderType type)
		{
			OPTICK_EVENT();

			pAllocator->GetDevice()->GetDeviceTable().vkCmdPushConstants(vCommandBuffer, pPipeline->GetPipelineLayout(), Utilities::GetShaderStage(type), static_cast<uint32_t>(pConstantData->m_Offset), static_cast<uint32_t>(pConstantData->m_Size), pConstantData->pData);
		}

		void VulkanCommandBuffer::BindDynamicStates(const VulkanComputePipeline* pPipeline, const DynamicStateContainer* pDynamicStates)
		{
			OPTICK_EVENT();

			if (!pDynamicStates)
				return;

			// Just bind the constant data as all the other dynamic states are not supported.
			BindConstantData(pPipeline, &pDynamicStates->m_ConstantBlocks[5]);
		}

		void VulkanCommandBuffer::BindConstantData(const VulkanComputePipeline* pPipeline, const ConstantData* pConstantData)
		{
			OPTICK_EVENT();

			pAllocator->GetDevice()->GetDeviceTable().vkCmdPushConstants(vCommandBuffer, pPipeline->GetPipelineLayout(), VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT, static_cast<uint32_t>(pConstantData->m_Offset), static_cast<uint32_t>(pConstantData->m_Size), pConstantData->pData);
		}

		void VulkanCommandBuffer::IssueDrawCall(WireFrame& wireFrame, const uint64_t firstInstance, const uint64_t instanceCount, const DrawCallMode mode)
		{
			OPTICK_EVENT();

			if (mode == DrawCallMode::Indexed)
				pAllocator->GetDevice()->GetDeviceTable().vkCmdDrawIndexed(vCommandBuffer, static_cast<uint32_t>(wireFrame.GetIndexCount()), static_cast<uint32_t>(instanceCount), static_cast<uint32_t>(wireFrame.GetIndexOffset()), static_cast<uint32_t>(wireFrame.GetVertexOffset()), static_cast<uint32_t>(firstInstance));

			else if (mode == DrawCallMode::Vertex)
				pAllocator->GetDevice()->GetDeviceTable().vkCmdDraw(vCommandBuffer, static_cast<uint32_t>(wireFrame.GetVertexCount()), static_cast<uint32_t>(instanceCount), static_cast<uint32_t>(wireFrame.GetVertexOffset()), static_cast<uint32_t>(firstInstance));

			else
				throw backend_error("Invalid draw call mode!");
		}

		void VulkanCommandBuffer::IssueDrawCall(WireFrame&& wireFrame, const uint64_t firstInstance, const uint64_t instanceCount, const DrawCallMode mode)
		{
			OPTICK_EVENT();

			if (mode == DrawCallMode::Indexed)
				pAllocator->GetDevice()->GetDeviceTable().vkCmdDrawIndexed(vCommandBuffer, static_cast<uint32_t>(wireFrame.GetIndexCount()), static_cast<uint32_t>(instanceCount), static_cast<uint32_t>(wireFrame.GetIndexOffset()), static_cast<uint32_t>(wireFrame.GetVertexOffset()), static_cast<uint32_t>(firstInstance));

			else if (mode == DrawCallMode::Vertex)
				pAllocator->GetDevice()->GetDeviceTable().vkCmdDraw(vCommandBuffer, static_cast<uint32_t>(wireFrame.GetVertexCount()), static_cast<uint32_t>(instanceCount), static_cast<uint32_t>(wireFrame.GetVertexOffset()), static_cast<uint32_t>(firstInstance));

			else
				throw backend_error("Invalid draw call mode!");
		}

		void VulkanCommandBuffer::IssueComputeCall(const FBox3D& groups)
		{
			OPTICK_EVENT();

			pAllocator->GetDevice()->GetDeviceTable().vkCmdDispatch(vCommandBuffer, groups.X, groups.Y, groups.Z);
		}

		void VulkanCommandBuffer::CopyImage(const VulkanImage* pSourceImage, const FBox3D sourceOffset, VulkanImage* pDestinationImage, const FBox3D destinationOffset)
		{
			OPTICK_EVENT();

			// Return if either one of images are null.
			if (!pSourceImage || !pDestinationImage)
				throw backend_error("One or more of the submitted images are null!");

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
			vLayers.layerCount = pSourceImage->GetLayerCount();
			vLayers.mipLevel = 0; // TODO
			vLayers.aspectMask = pSourceImage->GetAspectFlags();

			const auto vOldDstLayout = pDestinationImage->GetImageLayout();
			const auto vOldSrcLayout = pSourceImage->GetImageLayout();

			pDestinationImage->SetImageLayoutManual(vCommandBuffer, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
			pSourceImage->SetImageLayoutManual(vCommandBuffer, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);

			pDestinationImage->CopyFromImage(vCommandBuffer, pSourceImage->GetImage(), pSourceImage->GetImageLayout(), vSourceOffset, vDestinationOffset, vLayers);

			pDestinationImage->SetImageLayoutManual(vCommandBuffer, vOldDstLayout);
			pSourceImage->SetImageLayoutManual(vCommandBuffer, vOldSrcLayout);
		}

		void VulkanCommandBuffer::CopyToSwapChainImage(const VulkanImage* pSourceImage, const FBox3D sourceOffset, VulkanSwapChain* pSwapChain, const uint32_t imageIndex, const FBox3D destinationOffset)
		{
			OPTICK_EVENT();

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
			vLayers.layerCount = pSourceImage->GetLayerCount();
			vLayers.mipLevel = 0;
			//vLayers.mipLevel = vSourceImage.GetMipLevels();
			vLayers.aspectMask = pSourceImage->GetAspectFlags();

			const auto vOldSrcLayout = pSourceImage->GetImageLayout();

			pSourceImage->SetImageLayoutManual(vCommandBuffer, VkImageLayout::VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL);
			pSwapChain->CopyFromImage(vCommandBuffer, pSourceImage->GetImage(), pSourceImage->GetImageLayout(), vSourceOffset, vDestinationOffset, imageIndex, vLayers);
			pSourceImage->SetImageLayoutManual(vCommandBuffer, vOldSrcLayout);

			VkMemoryBarrier vMemoryBarrier = {};
			VkBufferMemoryBarrier vBufferMemoryBarrier = {};
			VkImageMemoryBarrier vImageMemoryBarrier = {};
		}

		void VulkanCommandBuffer::SubmitSecondaryCommandBuffer(const std::shared_ptr<VulkanCommandBuffer>& pCommandBuffer)
		{
			vSecondaryCommandBuffers.emplace_back(pCommandBuffer->GetVulkanCommandBuffer());
		}

		void VulkanCommandBuffer::ExecuteSecondaryCommands()
		{
			OPTICK_EVENT();

			if (!vSecondaryCommandBuffers.empty())
			{
				pAllocator->GetDevice()->GetDeviceTable().vkCmdExecuteCommands(vCommandBuffer, static_cast<uint32_t>(vSecondaryCommandBuffers.size()), vSecondaryCommandBuffers.data());
				vSecondaryCommandBuffers.clear();
			}
		}

		void VulkanCommandBuffer::EndBufferRecording()
		{
			OPTICK_EVENT();

			FLINT_VK_ASSERT(pAllocator->GetDevice()->GetDeviceTable().vkEndCommandBuffer(vCommandBuffer));
			bIsRecording = false;
		}

		void VulkanCommandBuffer::IncludeSwapChain(VulkanSwapChain* pSwapChain)
		{
			vInFlightSemaphores.emplace_back(pSwapChain->GetInFlightSemaphore());
			vRenderFinishedSemaphores.emplace_back(pSwapChain->GetRenderFinishedSemaphore());
		}

		void VulkanCommandBuffer::BeginQuery(const VulkanQuery* pQuery, const uint32_t index, const bool requirePrecision)
		{
			OPTICK_EVENT();

			pAllocator->GetDevice()->GetDeviceTable().vkCmdBeginQuery(vCommandBuffer, pQuery->GetQuery(), index, requirePrecision ? VkQueryControlFlagBits::VK_QUERY_CONTROL_PRECISE_BIT : 0);
		}

		void VulkanCommandBuffer::EndQuery(const VulkanQuery* pQuery, const uint32_t index)
		{
			OPTICK_EVENT();

			pAllocator->GetDevice()->GetDeviceTable().vkCmdEndQuery(vCommandBuffer, pQuery->GetQuery(), index);
		}

		void VulkanCommandBuffer::ResetQuery(const VulkanQuery* pQuery, const uint32_t beginIndex, const uint32_t count)
		{
			OPTICK_EVENT();

			pAllocator->GetDevice()->GetDeviceTable().vkCmdResetQueryPool(vCommandBuffer, pQuery->GetQuery(), beginIndex, count);
		}

		void VulkanCommandBuffer::Synchronize()
		{
			OPTICK_EVENT();

			VkMemoryBarrier vMemoryBarrier = {};
			VkBufferMemoryBarrier vBufferMemoryBarrier = {};
			VkImageMemoryBarrier vImageMemoryBarrier = {};

			pAllocator->GetDevice()->GetDeviceTable().vkCmdPipelineBarrier(
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