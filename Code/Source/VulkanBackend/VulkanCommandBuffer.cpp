// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanCommandBuffer.hpp"
#include "VulkanBackend/VulkanScreenBoundRenderTarget.hpp"
#include "VulkanBackend/VulkanOffScreenRenderTarget.hpp"
#include "VulkanBackend/VulkanGraphicsPipeline.hpp"
#include "VulkanBackend/VulkanComputePipeline.hpp"
#include "VulkanBackend/VulkanBuffer.hpp"
#include "VulkanBackend/VulkanUtilities.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanCommandBuffer::VulkanCommandBuffer(const std::shared_ptr<CommandBufferAllocator>& pAllocator, VkCommandBuffer vCommandBuffer)
			: CommandBuffer(pAllocator), vCommandBuffer(vCommandBuffer)
		{
		}

		void VulkanCommandBuffer::BeginBufferRecording()
		{
			FLINT_SETUP_PROFILER();

			VkCommandBufferBeginInfo vBeginInfo = {};
			vBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			vBeginInfo.pNext = VK_NULL_HANDLE;
			vBeginInfo.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			vBeginInfo.pInheritanceInfo = VK_NULL_HANDLE;

			FLINT_VK_ASSERT(vkBeginCommandBuffer(vCommandBuffer, &vBeginInfo));

			bIsRecording = true;
		}

		void VulkanCommandBuffer::BeginBufferRecording(const std::shared_ptr<CommandBufferList> pParent)
		{
			BeginBufferRecording();
		}

		void VulkanCommandBuffer::BindRenderTarget(const std::shared_ptr<ScreenBoundRenderTarget>& pRenderTarget)
		{
			FLINT_SETUP_PROFILER();

			VulkanScreenBoundRenderTarget& vRenderTarget = pRenderTarget->StaticCast<VulkanScreenBoundRenderTarget>();

			VkRenderPassBeginInfo vBeginInfo = {};
			vBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			vBeginInfo.pNext = VK_NULL_HANDLE;
			vBeginInfo.renderPass = vRenderTarget.GetRenderPass();
			vBeginInfo.framebuffer = vRenderTarget.GetFrameBuffer(vRenderTarget.GetFrameIndex());
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

			VkRenderPassBeginInfo vBeginInfo = {};
			vBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			vBeginInfo.pNext = VK_NULL_HANDLE;
			vBeginInfo.renderPass = vRenderTarget.GetRenderPass();
			vBeginInfo.framebuffer = vRenderTarget.GetFrameBuffer(vRenderTarget.GetFrameIndex());
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
			vBeginInfo.framebuffer = vRenderTarget.GetFrameBuffer(0);	// TODO
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
			vBeginInfo.framebuffer = vRenderTarget.GetFrameBuffer(0);	// TODO
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

		void VulkanCommandBuffer::BindVertexBuffer(const std::shared_ptr<Buffer>& pVertexBuffer)
		{
			FLINT_SETUP_PROFILER();

			if (pVertexBuffer->GetType() != BufferType::VERTEX)
				FLINT_THROW_INVALID_ARGUMENT("Invalid buffer type submitted to bind! Make sure that the buffer type is VERTEX.");

			VkDeviceSize offsets[1] = { 0 };
			vkCmdBindVertexBuffers(vCommandBuffer, 0, 1, pVertexBuffer->StaticCast<VulkanBuffer>().GetBufferAddress(), offsets);
		}

		void VulkanCommandBuffer::BindIndexBuffer(const std::shared_ptr<Buffer>& pIndexBuffer, UI64 indexSize)
		{
			FLINT_SETUP_PROFILER();

			if (pIndexBuffer->GetType() != BufferType::INDEX)
				FLINT_THROW_INVALID_ARGUMENT("Invalid buffer type submitted to bind! Make sure that the buffer type is INDEX.");

			VkIndexType indexType = VkIndexType::VK_INDEX_TYPE_UINT32;

			if (indexSize == sizeof(UI8))
				indexType = VkIndexType::VK_INDEX_TYPE_UINT8_EXT;
			else if (indexSize == sizeof(UI16))
				indexType = VkIndexType::VK_INDEX_TYPE_UINT16;
			else if (indexSize == sizeof(UI32))
				indexType = VkIndexType::VK_INDEX_TYPE_UINT32;
			else
				FLINT_THROW_INVALID_ARGUMENT("Invalid index size submitted to bind! The valid sizes are 1, 2, and 4 bytes.");

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
	}
}