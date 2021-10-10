// Copyright 2021 Dhiraj Wishal
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

#include "GraphicsCore/GeometryStore.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanCommandBuffer::VulkanCommandBuffer(const std::shared_ptr<CommandBufferAllocator>& pAllocator, VkCommandBuffer vCommandBuffer)
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
			vInFlightSemaphores.push_back(vSwapChain->GetInFlightSemaphore());
			vRenderFinishedSemaphores.push_back(vSwapChain->GetRenderFinishedSemaphore());

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
			vInFlightSemaphores.push_back(vSwapChain->GetInFlightSemaphore());
			vRenderFinishedSemaphores.push_back(vSwapChain->GetRenderFinishedSemaphore());

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

		void VulkanCommandBuffer::BindVertexBuffer(const Buffer* pBuffer, const UI64 firstBinding, const UI64 offset)
		{
			VkDeviceSize offsets[1] = { offset };
			pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetDeviceTable().vkCmdBindVertexBuffers(vCommandBuffer, static_cast<UI32>(firstBinding), 1, pBuffer->StaticCast<VulkanBuffer>().GetBufferAddress(), offsets);
		}

		void VulkanCommandBuffer::BindIndexBuffer(const Buffer* pBuffer, const UI64 indexSize, const UI64 offset)
		{
			VkIndexType indexType = VkIndexType::VK_INDEX_TYPE_UINT32;
			if (indexSize == sizeof(UI8))
				indexType = VkIndexType::VK_INDEX_TYPE_UINT8_EXT;

			else if (indexSize == sizeof(UI16))
				indexType = VkIndexType::VK_INDEX_TYPE_UINT16;

			else if (indexSize == sizeof(UI32))
				indexType = VkIndexType::VK_INDEX_TYPE_UINT32;

			else
				throw std::invalid_argument("Invalid index size submitted to bind! The valid sizes are 1, 2, and 4 bytes.");

			pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetDeviceTable().vkCmdBindIndexBuffer(vCommandBuffer, pBuffer->StaticCast<VulkanBuffer>().GetBuffer(), offset, indexType);
		}

		void VulkanCommandBuffer::BindGeometryStore(const GeometryStore* pGeometryStore)
		{
			OPTICK_EVENT();

			BindVertexBuffer(pGeometryStore->GetVertexBuffer().get());
			BindIndexBuffer(pGeometryStore->GetIndexBuffer().get(), pGeometryStore->GetIndexSize());
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
			for (UI64 i = 0; i < pResourcePackages.size(); i++)
			{
				auto& vPackage = pResourcePackages[i]->StaticCast<VulkanResourcePackage>();
				vPackage.PrepareIfNecessary();
				vDescriptorSets[i] = vPackage.GetDescriptorSet();
			}

			pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetDeviceTable().vkCmdBindDescriptorSets(vCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pPipeline->StaticCast<VulkanGraphicsPipeline>().GetPipelineLayout(), 0, static_cast<UI32>(vDescriptorSets.size()), vDescriptorSets.data(), 0, nullptr);
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
			for (UI64 i = 0; i < pResourcePackages.size(); i++)
			{
				auto& vPackage = pResourcePackages[i]->StaticCast<VulkanResourcePackage>();
				vPackage.PrepareIfNecessary();
				vDescriptorSets[i] = vPackage.GetDescriptorSet();
			}

			pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetDeviceTable().vkCmdBindDescriptorSets(vCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_COMPUTE, pPipeline->StaticCast<VulkanComputePipeline>().GetPipelineLayout(), 0, static_cast<UI32>(vDescriptorSets.size()), vDescriptorSets.data(), 0, nullptr);
		}

		void VulkanCommandBuffer::BindDynamicStates(const GraphicsPipeline* pPipeline, const DynamicStateContainer* pDynamicStates)
		{
			OPTICK_EVENT();

			if (!pDynamicStates)
				return;

			VulkanDevice& vDevice = pAllocator->GetDevice()->StaticCast<VulkanDevice>();
			DynamicStateFlags flags = pDynamicStates->mFlags;
			if (flags & DynamicStateFlags::ViewPort)
			{
				DynamicStateContainer::ViewPort const& viewPort = pDynamicStates->mViewPort;

				VkViewport vVP = {};
				vVP.width = viewPort.mExtent.mWidth;
				vVP.height = viewPort.mExtent.mHeight;
				vVP.minDepth = viewPort.mDepth.mWidth;
				vVP.maxDepth = viewPort.mDepth.mHeight;
				vVP.x = viewPort.mOffset.mWidth;
				vVP.y = viewPort.mOffset.mHeight;

				vDevice.GetDeviceTable().vkCmdSetViewport(vCommandBuffer, 0, 1, &vVP);
			}

			if (flags & DynamicStateFlags::Scissor)
			{
				DynamicStateContainer::Scissor const& scissor = pDynamicStates->mScissor;

				VkRect2D vR2D = {};
				vR2D.extent.width = scissor.mExtent.mWidth;
				vR2D.extent.height = scissor.mExtent.mHeight;
				vR2D.offset.x = scissor.mOffset.mWidth;
				vR2D.offset.y = scissor.mOffset.mHeight;

				vDevice.GetDeviceTable().vkCmdSetScissor(vCommandBuffer, 0, 1, &vR2D);
			}

			if (flags & DynamicStateFlags::LineWidth)
			{
				vkCmdSetLineWidth(vCommandBuffer, pDynamicStates->mLineWidth.mLineWidth);
			}

			if (flags & DynamicStateFlags::DepthBias)
			{
				DynamicStateContainer::DepthBias const& depthBias = pDynamicStates->mDepthBias;
				vDevice.GetDeviceTable().vkCmdSetDepthBias(vCommandBuffer, depthBias.mDepthBiasFactor, depthBias.mDepthClampFactor, depthBias.mDepthSlopeFactor);
			}

			if (flags & DynamicStateFlags::BlendConstants)
			{
				vDevice.GetDeviceTable().vkCmdSetBlendConstants(vCommandBuffer, pDynamicStates->mBlendConstants.mConstants);
			}

			if (flags & DynamicStateFlags::DepthBounds)
			{
				DynamicStateContainer::DepthBounds const& bounds = pDynamicStates->mDepthBounds;
				vDevice.GetDeviceTable().vkCmdSetDepthBounds(vCommandBuffer, bounds.mBounds.mWidth, bounds.mBounds.mHeight);
			}

			for (UI8 i = 0; i < 10; i++)
			{
				if (!pDynamicStates->mConstantBlocks[i].IsNull())
				{
					DynamicStateContainer::ConstantData const& data = pDynamicStates->mConstantBlocks[i];
					vDevice.GetDeviceTable().vkCmdPushConstants(vCommandBuffer, pPipeline->StaticCast<VulkanGraphicsPipeline>().GetPipelineLayout(), Utilities::GetShaderStage(ShaderType(i + 1)), static_cast<UI32>(data.mOffset), static_cast<UI32>(data.mSize), data.pData);
				}
			}
		}

		void VulkanCommandBuffer::BindDynamicStates(const ComputePipeline* pPipeline, const DynamicStateContainer* pDynamicStates)
		{
			OPTICK_EVENT();

			if (!pDynamicStates)
				return;

			VulkanDevice& vDevice = pAllocator->GetDevice()->StaticCast<VulkanDevice>();
			DynamicStateFlags flags = pDynamicStates->mFlags;
			if (flags & DynamicStateFlags::ViewPort)
			{
				DynamicStateContainer::ViewPort const& viewPort = pDynamicStates->mViewPort;

				VkViewport vVP = {};
				vVP.width = viewPort.mExtent.mWidth;
				vVP.height = viewPort.mExtent.mHeight;
				vVP.minDepth = viewPort.mDepth.mWidth;
				vVP.maxDepth = viewPort.mDepth.mHeight;
				vVP.x = viewPort.mOffset.mWidth;
				vVP.y = viewPort.mOffset.mHeight;

				vDevice.GetDeviceTable().vkCmdSetViewport(vCommandBuffer, 0, 1, &vVP);
			}

			if (flags & DynamicStateFlags::Scissor)
			{
				DynamicStateContainer::Scissor const& scissor = pDynamicStates->mScissor;

				VkRect2D vR2D = {};
				vR2D.extent.width = scissor.mExtent.mWidth;
				vR2D.extent.height = scissor.mExtent.mHeight;
				vR2D.offset.x = scissor.mOffset.mWidth;
				vR2D.offset.y = scissor.mOffset.mHeight;

				vDevice.GetDeviceTable().vkCmdSetScissor(vCommandBuffer, 0, 1, &vR2D);
			}

			if (flags & DynamicStateFlags::LineWidth)
			{
				vDevice.GetDeviceTable().vkCmdSetLineWidth(vCommandBuffer, pDynamicStates->mLineWidth.mLineWidth);
			}

			if (flags & DynamicStateFlags::DepthBias)
			{
				DynamicStateContainer::DepthBias const& depthBias = pDynamicStates->mDepthBias;
				vDevice.GetDeviceTable().vkCmdSetDepthBias(vCommandBuffer, depthBias.mDepthBiasFactor, depthBias.mDepthClampFactor, depthBias.mDepthSlopeFactor);
			}

			if (flags & DynamicStateFlags::BlendConstants)
			{
				vDevice.GetDeviceTable().vkCmdSetBlendConstants(vCommandBuffer, pDynamicStates->mBlendConstants.mConstants);
			}

			if (flags & DynamicStateFlags::DepthBounds)
			{
				DynamicStateContainer::DepthBounds const& bounds = pDynamicStates->mDepthBounds;
				vDevice.GetDeviceTable().vkCmdSetDepthBounds(vCommandBuffer, bounds.mBounds.mWidth, bounds.mBounds.mHeight);
			}

			// Just taking the 5th entry would be enough as it is the compute shader's block.
			if (!pDynamicStates->mConstantBlocks[5].IsNull())
			{
				DynamicStateContainer::ConstantData const& data = pDynamicStates->mConstantBlocks[5];
				vDevice.GetDeviceTable().vkCmdPushConstants(vCommandBuffer, pPipeline->StaticCast<VulkanComputePipeline>().GetPipelineLayout(), VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT, static_cast<UI32>(data.mOffset), static_cast<UI32>(data.mSize), data.pData);
			}
		}

		void VulkanCommandBuffer::IssueDrawCall(WireFrame& wireFrame, const UI64 firstInstance, const UI64 instanceCount, const DrawCallMode mode)
		{
			OPTICK_EVENT();

			VulkanDevice& vDevice = pAllocator->GetDevice()->StaticCast<VulkanDevice>();
			if (mode == DrawCallMode::Indexed)
				vDevice.GetDeviceTable().vkCmdDrawIndexed(vCommandBuffer, static_cast<UI32>(wireFrame.GetIndexCount()), static_cast<UI32>(instanceCount), static_cast<UI32>(wireFrame.GetIndexOffset()), static_cast<UI32>(wireFrame.GetVertexOffset()), static_cast<UI32>(firstInstance));

			else if (mode == DrawCallMode::Vertex)
				vDevice.GetDeviceTable().vkCmdDraw(vCommandBuffer, static_cast<UI32>(wireFrame.GetVertexCount()), static_cast<UI32>(instanceCount), static_cast<UI32>(wireFrame.GetVertexOffset()), static_cast<UI32>(firstInstance));

			else
				throw backend_error("Invalid draw call mode!");
		}

		void VulkanCommandBuffer::IssueDrawCall(WireFrame&& wireFrame, const UI64 firstInstance, const UI64 instanceCount, const DrawCallMode mode)
		{
			OPTICK_EVENT();

			VulkanDevice& vDevice = pAllocator->GetDevice()->StaticCast<VulkanDevice>();
			if (mode == DrawCallMode::Indexed)
				vDevice.GetDeviceTable().vkCmdDrawIndexed(vCommandBuffer, static_cast<UI32>(wireFrame.GetIndexCount()), static_cast<UI32>(instanceCount), static_cast<UI32>(wireFrame.GetIndexOffset()), static_cast<UI32>(wireFrame.GetVertexOffset()), static_cast<UI32>(firstInstance));

			else if (mode == DrawCallMode::Vertex)
				vDevice.GetDeviceTable().vkCmdDraw(vCommandBuffer, static_cast<UI32>(wireFrame.GetVertexCount()), static_cast<UI32>(instanceCount), static_cast<UI32>(wireFrame.GetVertexOffset()), static_cast<UI32>(firstInstance));

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

		void VulkanCommandBuffer::CopyToSwapChainImage(const Image* pSourceImage, const FBox3D sourceOffset, SwapChain* pSwapChain, const UI32 imageIndex, const FBox3D destinationOffset)
		{
			OPTICK_EVENT();

			auto const& vSourceImage = pSourceImage->StaticCast<VulkanImage>();
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
		}

		void VulkanCommandBuffer::SubmitSecondaryCommandBuffer(const std::shared_ptr<CommandBuffer>& pCommandBuffer)
		{
			vSecondaryCommandBuffers.push_back(pCommandBuffer->StaticCast<VulkanCommandBuffer>().GetVulkanCommandBuffer());
		}

		void VulkanCommandBuffer::ExecuteSecondaryCommands()
		{
			OPTICK_EVENT();

			if (!vSecondaryCommandBuffers.empty())
			{
				pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetDeviceTable().vkCmdExecuteCommands(vCommandBuffer, static_cast<UI32>(vSecondaryCommandBuffers.size()), vSecondaryCommandBuffers.data());
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
			vInFlightSemaphores.push_back(vSwapChain.GetInFlightSemaphore());
			vRenderFinishedSemaphores.push_back(vSwapChain.GetRenderFinishedSemaphore());
		}

		void VulkanCommandBuffer::Terminate()
		{
			bIsTerminated = true;
		}

		const VkSubmitInfo VulkanCommandBuffer::GetSubmitInfo() const
		{
			vSubmitInfo.signalSemaphoreCount = static_cast<UI32>(vRenderFinishedSemaphores.size());
			vSubmitInfo.pSignalSemaphores = vRenderFinishedSemaphores.data();
			vSubmitInfo.waitSemaphoreCount = static_cast<UI32>(vInFlightSemaphores.size());
			vSubmitInfo.pWaitSemaphores = vInFlightSemaphores.data();

			return vSubmitInfo;
		}

		const VkSubmitInfo* VulkanCommandBuffer::GetSubmitInfoAddress() const
		{
			vSubmitInfo.signalSemaphoreCount = static_cast<UI32>(vRenderFinishedSemaphores.size());
			vSubmitInfo.pSignalSemaphores = vRenderFinishedSemaphores.data();
			vSubmitInfo.waitSemaphoreCount = static_cast<UI32>(vInFlightSemaphores.size());
			vSubmitInfo.pWaitSemaphores = vInFlightSemaphores.data();

			return &vSubmitInfo;
		}
	}
}