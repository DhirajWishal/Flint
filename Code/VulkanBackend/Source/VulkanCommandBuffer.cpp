// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanCommandBuffer.hpp"
#include "VulkanBackend/VulkanCommandBufferAllocator.hpp"
#include "VulkanBackend/VulkanScreenBoundRenderTarget.hpp"
#include "VulkanBackend/VulkanOffScreenRenderTarget.hpp"
#include "VulkanBackend/VulkanGraphicsPipeline.hpp"
#include "VulkanBackend/VulkanComputePipeline.hpp"
#include "VulkanBackend/VulkanBuffer.hpp"
#include "VulkanBackend/VulkanUtilities.hpp"

#include "GraphicsCore/GeometryStore.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanCommandBuffer::VulkanCommandBuffer(const std::shared_ptr<CommandBufferAllocator>& pAllocator, VkCommandBuffer vCommandBuffer)
			: CommandBuffer(pAllocator), vCommandBuffer(vCommandBuffer)
		{
			FLINT_SETUP_PROFILER();

			VkSemaphoreCreateInfo vSemaphoreCreateInfo = {};
			vSemaphoreCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
			vSemaphoreCreateInfo.pNext = VK_NULL_HANDLE;
			vSemaphoreCreateInfo.flags = 0;

			VkFenceCreateInfo vFenceCreateInfo = {};
			vFenceCreateInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
			vFenceCreateInfo.pNext = VK_NULL_HANDLE;
			vFenceCreateInfo.flags = VkFenceCreateFlagBits::VK_FENCE_CREATE_SIGNALED_BIT;

			FLINT_VK_ASSERT(vkCreateFence(pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetLogicalDevice(), &vFenceCreateInfo, nullptr, &vInFlightFence));

			vSubmitInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SUBMIT_INFO;
			vSubmitInfo.pNext = VK_NULL_HANDLE;
			vSubmitInfo.commandBufferCount = 1;
			vSubmitInfo.pCommandBuffers = &this->vCommandBuffer;
			vSubmitInfo.pWaitDstStageMask = &vWaitStage;
		}

		void VulkanCommandBuffer::BeginBufferRecording()
		{
			FLINT_SETUP_PROFILER();

			VkCommandBufferBeginInfo vBeginInfo = {};
			vBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			vBeginInfo.pNext = VK_NULL_HANDLE;
			vBeginInfo.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			vBeginInfo.pInheritanceInfo = VK_NULL_HANDLE;

			//FLINT_VK_ASSERT(vkWaitForFences(pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetLogicalDevice(), 1, &vInFlightFence, VK_TRUE, UI64_MAX));
			FLINT_VK_ASSERT(vkBeginCommandBuffer(vCommandBuffer, &vBeginInfo));

			vInFlightSemaphores.clear();
			vRenderFinishedSemaphores.clear();

			bIsRecording = true;
		}

		void VulkanCommandBuffer::BeginBufferRecording(const std::shared_ptr<ScreenBoundRenderTarget> pRenderTarget)
		{
			FLINT_SETUP_PROFILER();

			VkCommandBufferBeginInfo vBeginInfo = {};
			vBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			vBeginInfo.pNext = VK_NULL_HANDLE;
			vBeginInfo.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			vBeginInfo.pInheritanceInfo = pRenderTarget->StaticCast<VulkanScreenBoundRenderTarget>().GetVulkanInheritanceInfo();

			//FLINT_VK_ASSERT(vkWaitForFences(pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetLogicalDevice(), 1, &vInFlightFence, VK_TRUE, UI64_MAX));
			FLINT_VK_ASSERT(vkBeginCommandBuffer(vCommandBuffer, &vBeginInfo));

			bIsRecording = true;
		}

		void VulkanCommandBuffer::BeginBufferRecording(const std::shared_ptr<OffScreenRenderTarget> pRenderTarget)
		{
			FLINT_SETUP_PROFILER();

			VkCommandBufferBeginInfo vBeginInfo = {};
			vBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			vBeginInfo.pNext = VK_NULL_HANDLE;
			vBeginInfo.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			vBeginInfo.pInheritanceInfo = pRenderTarget->StaticCast<VulkanOffScreenRenderTarget>().GetVulkanInheritanceInfo();

			//FLINT_VK_ASSERT(vkWaitForFences(pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetLogicalDevice(), 1, &vInFlightFence, VK_TRUE, UI64_MAX));
			FLINT_VK_ASSERT(vkBeginCommandBuffer(vCommandBuffer, &vBeginInfo));

			bIsRecording = true;
		}

		void VulkanCommandBuffer::BindRenderTarget(const std::shared_ptr<ScreenBoundRenderTarget>& pRenderTarget)
		{
			FLINT_SETUP_PROFILER();

			VulkanScreenBoundRenderTarget& vRenderTarget = pRenderTarget->StaticCast<VulkanScreenBoundRenderTarget>();

			vInFlightSemaphores.push_back(vRenderTarget.GetSwapChain()->GetInFlightSemaphore());
			vRenderFinishedSemaphores.push_back(vRenderTarget.GetSwapChain()->GetRenderFinishedSemaphore());

			VkRenderPassBeginInfo vBeginInfo = {};
			vBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			vBeginInfo.pNext = VK_NULL_HANDLE;
			vBeginInfo.renderPass = vRenderTarget.GetRenderPass();
			vBeginInfo.framebuffer = vRenderTarget.GetFramebuffer();
			vBeginInfo.clearValueCount = vRenderTarget.GetClearScreenValueCount();
			vBeginInfo.pClearValues = vRenderTarget.GetClearScreenValues();
			vBeginInfo.renderArea.extent.width = vRenderTarget.GetExtent().mWidth;
			vBeginInfo.renderArea.extent.height = vRenderTarget.GetExtent().mHeight;

			vkCmdBeginRenderPass(vCommandBuffer, &vBeginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
		}

		void VulkanCommandBuffer::BindRenderTargetSecondary(const std::shared_ptr<ScreenBoundRenderTarget>& pRenderTarget)
		{
			FLINT_SETUP_PROFILER();

			VulkanScreenBoundRenderTarget& vRenderTarget = pRenderTarget->StaticCast<VulkanScreenBoundRenderTarget>();

			vInFlightSemaphores.push_back(vRenderTarget.GetSwapChain()->GetInFlightSemaphore());
			vRenderFinishedSemaphores.push_back(vRenderTarget.GetSwapChain()->GetRenderFinishedSemaphore());

			VkRenderPassBeginInfo vBeginInfo = {};
			vBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			vBeginInfo.pNext = VK_NULL_HANDLE;
			vBeginInfo.renderPass = vRenderTarget.GetRenderPass();
			vBeginInfo.framebuffer = vRenderTarget.GetFramebuffer();
			vBeginInfo.clearValueCount = vRenderTarget.GetClearScreenValueCount();
			vBeginInfo.pClearValues = vRenderTarget.GetClearScreenValues();
			vBeginInfo.renderArea.extent.width = vRenderTarget.GetExtent().mWidth;
			vBeginInfo.renderArea.extent.height = vRenderTarget.GetExtent().mHeight;

			vkCmdBeginRenderPass(vCommandBuffer, &vBeginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
		}

		void VulkanCommandBuffer::BindRenderTarget(const std::shared_ptr<OffScreenRenderTarget>& pRenderTarget)
		{
			FLINT_SETUP_PROFILER();

			VulkanOffScreenRenderTarget& vRenderTarget = pRenderTarget->StaticCast<VulkanOffScreenRenderTarget>();

			VkRenderPassBeginInfo vBeginInfo = {};
			vBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			vBeginInfo.pNext = VK_NULL_HANDLE;
			vBeginInfo.renderPass = vRenderTarget.GetRenderPass();
			vBeginInfo.framebuffer = vRenderTarget.GetFramebuffer();
			vBeginInfo.clearValueCount = vRenderTarget.GetClearScreenValueCount();
			vBeginInfo.pClearValues = vRenderTarget.GetClearScreenValues();
			vBeginInfo.renderArea.extent.width = vRenderTarget.GetExtent().mWidth;
			vBeginInfo.renderArea.extent.height = vRenderTarget.GetExtent().mHeight;

			vkCmdBeginRenderPass(vCommandBuffer, &vBeginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
		}

		void VulkanCommandBuffer::BindRenderTargetSecondary(const std::shared_ptr<OffScreenRenderTarget>& pRenderTarget)
		{
			FLINT_SETUP_PROFILER();

			VulkanOffScreenRenderTarget& vRenderTarget = pRenderTarget->StaticCast<VulkanOffScreenRenderTarget>();

			VkRenderPassBeginInfo vBeginInfo = {};
			vBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			vBeginInfo.pNext = VK_NULL_HANDLE;
			vBeginInfo.renderPass = vRenderTarget.GetRenderPass();
			vBeginInfo.framebuffer = vRenderTarget.GetFramebuffer();
			vBeginInfo.clearValueCount = vRenderTarget.GetClearScreenValueCount();
			vBeginInfo.pClearValues = vRenderTarget.GetClearScreenValues();
			vBeginInfo.renderArea.extent.width = vRenderTarget.GetExtent().mWidth;
			vBeginInfo.renderArea.extent.height = vRenderTarget.GetExtent().mHeight;

			vkCmdBeginRenderPass(vCommandBuffer, &vBeginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
		}

		void VulkanCommandBuffer::UnbindRenderTarget()
		{
			FLINT_SETUP_PROFILER();

			vkCmdEndRenderPass(vCommandBuffer);
		}

		void VulkanCommandBuffer::BindGraphicsPipeline(const std::shared_ptr<GraphicsPipeline>& pGraphicsPipeline)
		{
			FLINT_SETUP_PROFILER();

			vkCmdBindPipeline(vCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pGraphicsPipeline->StaticCast<VulkanGraphicsPipeline>().GetPipeline());
		}

		void VulkanCommandBuffer::BindComputePipeline(const std::shared_ptr<ComputePipeline>& pComputePipeline)
		{
			FLINT_SETUP_PROFILER();

			vkCmdBindPipeline(vCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_COMPUTE, pComputePipeline->StaticCast<VulkanComputePipeline>().GetPipeline());
		}

		void VulkanCommandBuffer::BindGeometryStore(const std::shared_ptr<GeometryStore>& pGeometryStore)
		{
			FLINT_SETUP_PROFILER();

			auto pVertexBuffer = pGeometryStore->GetVertexBuffer();
			auto pIndexBuffer = pGeometryStore->GetIndexBuffer();

			UI64 indexSize = pGeometryStore->GetIndexSize();
			VkIndexType indexType = VkIndexType::VK_INDEX_TYPE_UINT32;

			if (indexSize == sizeof(UI8))
				indexType = VkIndexType::VK_INDEX_TYPE_UINT8_EXT;
			else if (indexSize == sizeof(UI16))
				indexType = VkIndexType::VK_INDEX_TYPE_UINT16;
			else if (indexSize == sizeof(UI32))
				indexType = VkIndexType::VK_INDEX_TYPE_UINT32;
			else
				FLINT_THROW_INVALID_ARGUMENT("Invalid index size submitted to bind! The valid sizes are 1, 2, and 4 bytes.");

			VkDeviceSize offsets[1] = { 0 };
			vkCmdBindVertexBuffers(vCommandBuffer, 0, 1, pVertexBuffer->StaticCast<VulkanBuffer>().GetBufferAddress(), offsets);
			vkCmdBindIndexBuffer(vCommandBuffer, pIndexBuffer->StaticCast<VulkanBuffer>().GetBuffer(), 0, indexType);
		}

		void VulkanCommandBuffer::BindResourceMap(const std::shared_ptr<GraphicsPipeline>& pPipeline, const std::shared_ptr<ResourceMap>& pResourceMap)
		{
			FLINT_SETUP_PROFILER();

			const VulkanGraphicsPipeline& vPipeline = pPipeline->StaticCast<VulkanGraphicsPipeline>();
			const VkDescriptorSet* pDescriptorSet = vPipeline.GetDescriptorSetAddress(pResourceMap);

			if (pDescriptorSet)
				vkCmdBindDescriptorSets(vCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, vPipeline.GetPipelineLayout(), 0, 1, pDescriptorSet, 0, nullptr);
		}

		void VulkanCommandBuffer::BindResourceMap(const std::shared_ptr<ComputePipeline>& pPipeline, const std::shared_ptr<ResourceMap>& pResourceMap)
		{
			FLINT_SETUP_PROFILER();

			const VulkanComputePipeline& vPipeline = pPipeline->StaticCast<VulkanComputePipeline>();
			const VkDescriptorSet* pDescriptorSet = vPipeline.GetDescriptorSetAddress(pResourceMap);

			if (pDescriptorSet)
				vkCmdBindDescriptorSets(vCommandBuffer, VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_COMPUTE, vPipeline.GetPipelineLayout(), 0, 1, pDescriptorSet, 0, nullptr);
		}

		void VulkanCommandBuffer::BindDynamicStates(const std::shared_ptr<GraphicsPipeline>& pPipeline, const std::shared_ptr<DynamicStateContainer>& pDynamicStates)
		{
			FLINT_SETUP_PROFILER();

			if (!pDynamicStates)
				return;

			DynamicStateFlags flags = pDynamicStates->mFlags;
			if (flags & DynamicStateFlags::VIEWPORT)
			{
				DynamicStateContainer::ViewPort& viewPort = pDynamicStates->mViewPort;

				VkViewport vVP = {};
				vVP.width = viewPort.mExtent.mWidth;
				vVP.height = viewPort.mExtent.mHeight;
				vVP.minDepth = viewPort.mDepth.mWidth;
				vVP.maxDepth = viewPort.mDepth.mHeight;
				vVP.x = viewPort.mOffset.mWidth;
				vVP.y = viewPort.mOffset.mHeight;

				vkCmdSetViewport(vCommandBuffer, 0, 1, &vVP);
			}

			if (flags & DynamicStateFlags::SCISSOR)
			{
				DynamicStateContainer::Scissor& scissor = pDynamicStates->mScissor;

				VkRect2D vR2D = {};
				vR2D.extent.width = scissor.mExtent.mWidth;
				vR2D.extent.height = scissor.mExtent.mHeight;
				vR2D.offset.x = scissor.mOffset.mWidth;
				vR2D.offset.y = scissor.mOffset.mHeight;

				vkCmdSetScissor(vCommandBuffer, 0, 1, &vR2D);
			}

			if (flags & DynamicStateFlags::LINE_WIDTH)
			{
				vkCmdSetLineWidth(vCommandBuffer, pDynamicStates->mLineWidth.mLineWidth);
			}

			if (flags & DynamicStateFlags::DEPTH_BIAS)
			{
				DynamicStateContainer::DepthBias& depthBias = pDynamicStates->mDepthBias;
				vkCmdSetDepthBias(vCommandBuffer, depthBias.mDepthBiasFactor, depthBias.mDepthClampFactor, depthBias.mDepthSlopeFactor);
			}

			if (flags & DynamicStateFlags::BLEND_CONSTANTS)
			{
				vkCmdSetBlendConstants(vCommandBuffer, pDynamicStates->mBlendConstants.mConstants);
			}

			if (flags & DynamicStateFlags::DEPTH_BOUNDS)
			{
				DynamicStateContainer::DepthBounds& bounds = pDynamicStates->mDepthBounds;
				vkCmdSetDepthBounds(vCommandBuffer, bounds.mBounds.mWidth, bounds.mBounds.mHeight);
			}

			for (UI8 i = 0; i < 10; i++)
			{
				if (!pDynamicStates->mConstantBlocks[i].IsNull())
				{
					DynamicStateContainer::ConstantData& data = pDynamicStates->mConstantBlocks[i];
					vkCmdPushConstants(vCommandBuffer, pPipeline->StaticCast<VulkanGraphicsPipeline>().GetPipelineLayout(), Utilities::GetShaderStage(ShaderType(i + 1)), static_cast<UI32>(data.mOffset), static_cast<UI32>(data.mSize), data.pData);
				}
			}
		}

		void VulkanCommandBuffer::BindDynamicStates(const std::shared_ptr<ComputePipeline>& pPipeline, const std::shared_ptr<DynamicStateContainer>& pDynamicStates)
		{
			FLINT_SETUP_PROFILER();

			if (!pDynamicStates)
				return;

			DynamicStateFlags flags = pDynamicStates->mFlags;
			if (flags & DynamicStateFlags::VIEWPORT)
			{
				DynamicStateContainer::ViewPort& viewPort = pDynamicStates->mViewPort;

				VkViewport vVP = {};
				vVP.width = viewPort.mExtent.mWidth;
				vVP.height = viewPort.mExtent.mHeight;
				vVP.minDepth = viewPort.mDepth.mWidth;
				vVP.maxDepth = viewPort.mDepth.mHeight;
				vVP.x = viewPort.mOffset.mWidth;
				vVP.y = viewPort.mOffset.mHeight;

				vkCmdSetViewport(vCommandBuffer, 0, 1, &vVP);
			}

			if (flags & DynamicStateFlags::SCISSOR)
			{
				DynamicStateContainer::Scissor& scissor = pDynamicStates->mScissor;

				VkRect2D vR2D = {};
				vR2D.extent.width = scissor.mExtent.mWidth;
				vR2D.extent.height = scissor.mExtent.mHeight;
				vR2D.offset.x = scissor.mOffset.mWidth;
				vR2D.offset.y = scissor.mOffset.mHeight;

				vkCmdSetScissor(vCommandBuffer, 0, 1, &vR2D);
			}

			if (flags & DynamicStateFlags::LINE_WIDTH)
			{
				vkCmdSetLineWidth(vCommandBuffer, pDynamicStates->mLineWidth.mLineWidth);
			}

			if (flags & DynamicStateFlags::DEPTH_BIAS)
			{
				DynamicStateContainer::DepthBias& depthBias = pDynamicStates->mDepthBias;
				vkCmdSetDepthBias(vCommandBuffer, depthBias.mDepthBiasFactor, depthBias.mDepthClampFactor, depthBias.mDepthSlopeFactor);
			}

			if (flags & DynamicStateFlags::BLEND_CONSTANTS)
			{
				vkCmdSetBlendConstants(vCommandBuffer, pDynamicStates->mBlendConstants.mConstants);
			}

			if (flags & DynamicStateFlags::DEPTH_BOUNDS)
			{
				DynamicStateContainer::DepthBounds& bounds = pDynamicStates->mDepthBounds;
				vkCmdSetDepthBounds(vCommandBuffer, bounds.mBounds.mWidth, bounds.mBounds.mHeight);
			}

			for (UI8 i = 0; i < 10; i++)
			{
				if (!pDynamicStates->mConstantBlocks[i].IsNull())
				{
					DynamicStateContainer::ConstantData& data = pDynamicStates->mConstantBlocks[i];
					vkCmdPushConstants(vCommandBuffer, pPipeline->StaticCast<VulkanComputePipeline>().GetPipelineLayout(), Utilities::GetShaderStage(ShaderType(i + 1)), static_cast<UI32>(data.mOffset), static_cast<UI32>(data.mSize), data.pData);
				}
			}
		}

		void VulkanCommandBuffer::IssueDrawCall(UI64 vertexOffset, UI64 vertexCount, UI64 indexOffset, UI64 indexCount)
		{
			FLINT_SETUP_PROFILER();

			vkCmdDrawIndexed(vCommandBuffer, static_cast<UI32>(indexCount), 1, static_cast<UI32>(indexOffset), static_cast<UI32>(vertexOffset), 0);
		}

		void VulkanCommandBuffer::IssueComputeCall(const FBox3D& groups)
		{
			FLINT_SETUP_PROFILER();

			vkCmdDispatch(vCommandBuffer, groups.X, groups.Y, groups.Z);
		}

		void VulkanCommandBuffer::SubmitSecondaryCommandBuffer(const std::shared_ptr<CommandBuffer>& pCommandBuffer)
		{
			vSecondaryCommandBuffers.push_back(pCommandBuffer->StaticCast<VulkanCommandBuffer>().GetVulkanCommandBuffer());
		}

		void VulkanCommandBuffer::ExecuteSecondaryCommands()
		{
			FLINT_SETUP_PROFILER();

			if (!vSecondaryCommandBuffers.empty())
				vkCmdExecuteCommands(vCommandBuffer, static_cast<UI32>(vSecondaryCommandBuffers.size()), vSecondaryCommandBuffers.data());

			vSecondaryCommandBuffers.clear();
		}

		void VulkanCommandBuffer::EndBufferRecording()
		{
			FLINT_SETUP_PROFILER();

			FLINT_VK_ASSERT(vkEndCommandBuffer(vCommandBuffer));
			bIsRecording = false;
		}

		void VulkanCommandBuffer::Terminate()
		{
			auto& vDevice = pAllocator->GetDevice()->StaticCast<VulkanDevice>();

			vkDestroyFence(vDevice.GetLogicalDevice(), vInFlightFence, nullptr);
			bIsTerminated = true;
		}

		const VkSubmitInfo VulkanCommandBuffer::GetSubmitInfo()
		{
			vSubmitInfo.signalSemaphoreCount = static_cast<UI32>(vRenderFinishedSemaphores.size());
			vSubmitInfo.pSignalSemaphores = vRenderFinishedSemaphores.data();
			vSubmitInfo.waitSemaphoreCount = static_cast<UI32>(vInFlightSemaphores.size());
			vSubmitInfo.pWaitSemaphores = vInFlightSemaphores.data();

			FLINT_VK_ASSERT(vkResetFences(pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetLogicalDevice(), 1, &vInFlightFence));
			return vSubmitInfo;
		}

		void VulkanCommandBuffer::ResetFence()
		{
			FLINT_VK_ASSERT(vkResetFences(pAllocator->GetDevice()->StaticCast<VulkanDevice>().GetLogicalDevice(), 1, &vInFlightFence));
		}
	}
}