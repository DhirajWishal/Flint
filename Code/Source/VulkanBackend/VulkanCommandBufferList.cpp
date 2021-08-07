// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanCommandBufferList.hpp"
#include "VulkanBackend/VulkanScreenBoundRenderTarget.hpp"
#include "VulkanBackend/VulkanGraphicsPipeline.hpp"
#include "VulkanBackend/VulkanBuffer.hpp"
#include "VulkanBackend/VulkanUtilities.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanCommandBufferList::VulkanCommandBufferList(const std::shared_ptr<Device>& pDevice, const UI32 bufferCount) : CommandBufferList(pDevice, bufferCount)
		{
			FLINT_SETUP_PROFILER();

			auto vDevice = pDevice->StaticCast<VulkanDevice>();

			VkCommandPoolCreateInfo vPoolCI = {};
			vPoolCI.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			vPoolCI.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			vPoolCI.pNext = VK_NULL_HANDLE;
			vPoolCI.queueFamilyIndex = vDevice.GetQueue().mTransferFamily.value();

			FLINT_VK_ASSERT(vkCreateCommandPool(vDevice.GetLogicalDevice(), &vPoolCI, nullptr, &vCommandPool));

			VkCommandBufferAllocateInfo vAI = {};
			vAI.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			vAI.pNext = VK_NULL_HANDLE;
			vAI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			vAI.commandPool = vCommandPool;
			vAI.commandBufferCount = bufferCount;

			vCommandBuffers.resize(bufferCount);
			FLINT_VK_ASSERT(vkAllocateCommandBuffers(vDevice.GetLogicalDevice(), &vAI, vCommandBuffers.data()));
		}

		VulkanCommandBufferList::VulkanCommandBufferList(const std::shared_ptr<Device>& pDevice, const UI32 bufferCount, const std::shared_ptr<CommandBufferList>& pParent)
			: CommandBufferList(pDevice, bufferCount, pParent)
		{
			FLINT_SETUP_PROFILER();

			auto vDevice = pDevice->StaticCast<VulkanDevice>();

			VkCommandPoolCreateInfo vPoolCI = {};
			vPoolCI.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			vPoolCI.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			vPoolCI.pNext = VK_NULL_HANDLE;
			vPoolCI.queueFamilyIndex = vDevice.GetQueue().mTransferFamily.value();

			FLINT_VK_ASSERT(vkCreateCommandPool(vDevice.GetLogicalDevice(), &vPoolCI, nullptr, &vCommandPool));

			VkCommandBufferAllocateInfo vAI = {};
			vAI.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			vAI.pNext = VK_NULL_HANDLE;
			vAI.level = VK_COMMAND_BUFFER_LEVEL_SECONDARY;
			vAI.commandPool = vCommandPool;
			vAI.commandBufferCount = bufferCount;

			vCommandBuffers.resize(bufferCount);
			FLINT_VK_ASSERT(vkAllocateCommandBuffers(vDevice.GetLogicalDevice(), &vAI, vCommandBuffers.data()));
		}

		std::shared_ptr<CommandBufferList> VulkanCommandBufferList::CreateChild()
		{
			return pDevice->CreateSecondaryCommandBufferList(mBufferCount, shared_from_this());
		}

		void VulkanCommandBufferList::BeginBufferRecording(UI32 index)
		{
			FLINT_SETUP_PROFILER();

			mCurrentBufferIndex = index;
			vCurrentBuffer = vCommandBuffers[index];

			VkCommandBufferBeginInfo vBeginInfo = {};
			vBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			vBeginInfo.pNext = VK_NULL_HANDLE;
			vBeginInfo.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			vBeginInfo.pInheritanceInfo = VK_NULL_HANDLE;

			FLINT_VK_ASSERT(vkBeginCommandBuffer(vCurrentBuffer, &vBeginInfo));

			bIsRecording = true;
		}

		void VulkanCommandBufferList::BeginBufferRecording(UI32 index, const std::shared_ptr<CommandBufferList> pParent)
		{
			BeginBufferRecording(index);
		}

		void VulkanCommandBufferList::BeginNextBufferRecording()
		{
			FLINT_SETUP_PROFILER();

			mCurrentBufferIndex++;
			if (mCurrentBufferIndex >= mBufferCount) mCurrentBufferIndex = 0;

			BeginBufferRecording(mCurrentBufferIndex);
		}

		void VulkanCommandBufferList::BeginNextBufferRecording(const std::shared_ptr<CommandBufferList> pParent)
		{
			FLINT_SETUP_PROFILER();

			mCurrentBufferIndex++;
			if (mCurrentBufferIndex >= mBufferCount) mCurrentBufferIndex = 0;

			BeginBufferRecording(mCurrentBufferIndex, pParent);
		}

		void VulkanCommandBufferList::BindRenderTarget(const std::shared_ptr<ScreenBoundRenderTarget>& pRenderTarget)
		{
			FLINT_SETUP_PROFILER();

			VulkanScreenBoundRenderTarget& vRenderTarget = pRenderTarget->StaticCast<VulkanScreenBoundRenderTarget>();
			FColor4D clearColors = vRenderTarget.GetClearColor();

			VkRenderPassBeginInfo vBeginInfo = {};
			vBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			vBeginInfo.pNext = VK_NULL_HANDLE;
			vBeginInfo.renderPass = vRenderTarget.GetRenderPass();
			vBeginInfo.framebuffer = vRenderTarget.GetFrameBuffer(GetCurrentBufferIndex());
			vBeginInfo.clearValueCount = vRenderTarget.GetClearScreenValueCount();
			vBeginInfo.pClearValues = vRenderTarget.GetClearScreenValues();
			vBeginInfo.renderArea.extent.width = vRenderTarget.GetExtent().mWidth;
			vBeginInfo.renderArea.extent.height = vRenderTarget.GetExtent().mHeight;

			vkCmdBeginRenderPass(GetCurrentCommandBuffer(), &vBeginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_INLINE);
		}

		void VulkanCommandBufferList::BindRenderTargetSecondary(const std::shared_ptr<ScreenBoundRenderTarget>& pRenderTarget)
		{
			FLINT_SETUP_PROFILER();

			VulkanScreenBoundRenderTarget& vRenderTarget = pRenderTarget->StaticCast<VulkanScreenBoundRenderTarget>();
			FColor4D clearColors = vRenderTarget.GetClearColor();

			VkRenderPassBeginInfo vBeginInfo = {};
			vBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			vBeginInfo.pNext = VK_NULL_HANDLE;
			vBeginInfo.renderPass = vRenderTarget.GetRenderPass();
			vBeginInfo.framebuffer = vRenderTarget.GetFrameBuffer(GetCurrentBufferIndex());
			vBeginInfo.clearValueCount = vRenderTarget.GetClearScreenValueCount();
			vBeginInfo.pClearValues = vRenderTarget.GetClearScreenValues();
			vBeginInfo.renderArea.extent.width = vRenderTarget.GetExtent().mWidth;
			vBeginInfo.renderArea.extent.height = vRenderTarget.GetExtent().mHeight;

			vkCmdBeginRenderPass(GetCurrentCommandBuffer(), &vBeginInfo, VkSubpassContents::VK_SUBPASS_CONTENTS_SECONDARY_COMMAND_BUFFERS);
		}

		void VulkanCommandBufferList::UnbindRenderTarget()
		{
			FLINT_SETUP_PROFILER();

			vkCmdEndRenderPass(GetCurrentCommandBuffer());
		}

		void VulkanCommandBufferList::BindGraphicsPipeline(const std::shared_ptr<GraphicsPipeline>& pGraphicsPipeline)
		{
			FLINT_SETUP_PROFILER();

			vkCmdBindPipeline(GetCurrentCommandBuffer(), VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, pGraphicsPipeline->StaticCast<VulkanGraphicsPipeline>().GetPipeline());
		}

		void VulkanCommandBufferList::BindVertexBuffer(const std::shared_ptr<Buffer>& pVertexBuffer)
		{
			FLINT_SETUP_PROFILER();

			if (pVertexBuffer->GetType() != BufferType::VERTEX)
				FLINT_THROW_INVALID_ARGUMENT("Invalid buffer type submitted to bind! Make sure that the buffer type is VERTEX.");

			VkDeviceSize offsets[1] = { 0 };
			vkCmdBindVertexBuffers(GetCurrentCommandBuffer(), 0, 1, pVertexBuffer->StaticCast<VulkanBuffer>().GetBufferAddress(), offsets);
		}

		void VulkanCommandBufferList::BindIndexBuffer(const std::shared_ptr<Buffer>& pIndexBuffer, UI64 indexSize)
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

			vkCmdBindIndexBuffer(GetCurrentCommandBuffer(), pIndexBuffer->StaticCast<VulkanBuffer>().GetBuffer(), 0, indexType);
		}

		void VulkanCommandBufferList::BindDrawResources(const std::shared_ptr<GraphicsPipeline>& pPipeline, const std::shared_ptr<ResourceMap>& pResourceMap)
		{
			FLINT_SETUP_PROFILER();

			const VulkanGraphicsPipeline& vPipeline = pPipeline->StaticCast<VulkanGraphicsPipeline>();
			const VkDescriptorSet* pDescriptorSet = vPipeline.GetDescriptorSetAddress(pResourceMap);

			if (pDescriptorSet)
				vkCmdBindDescriptorSets(GetCurrentCommandBuffer(), VkPipelineBindPoint::VK_PIPELINE_BIND_POINT_GRAPHICS, vPipeline.GetPipelineLayout(), 0, 1, pDescriptorSet, 0, nullptr);
		}

		void VulkanCommandBufferList::BindDynamicStates(const std::shared_ptr<GraphicsPipeline>& pPipeline, const std::shared_ptr<DynamicStateContainer>& pDynamicStates)
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

				vkCmdSetViewport(GetCurrentCommandBuffer(), 0, 1, &vVP);
			}

			if (flags & DynamicStateFlags::SCISSOR)
			{
				DynamicStateContainer::Scissor& scissor = pDynamicStates->mScissor;

				VkRect2D vR2D = {};
				vR2D.extent.width = scissor.mExtent.mWidth;
				vR2D.extent.height = scissor.mExtent.mHeight;
				vR2D.offset.x = scissor.mOffset.mWidth;
				vR2D.offset.y = scissor.mOffset.mHeight;

				vkCmdSetScissor(GetCurrentCommandBuffer(), 0, 1, &vR2D);
			}

			if (flags & DynamicStateFlags::LINE_WIDTH)
			{
				vkCmdSetLineWidth(GetCurrentCommandBuffer(), pDynamicStates->mLineWidth.mLineWidth);
			}

			if (flags & DynamicStateFlags::DEPTH_BIAS)
			{
				DynamicStateContainer::DepthBias& depthBias = pDynamicStates->mDepthBias;
				vkCmdSetDepthBias(GetCurrentCommandBuffer(), depthBias.mDepthBiasFactor, depthBias.mDepthClampFactor, depthBias.mDepthSlopeFactor);
			}

			if (flags & DynamicStateFlags::BLEND_CONSTANTS)
			{
				vkCmdSetBlendConstants(GetCurrentCommandBuffer(), pDynamicStates->mBlendConstants.mConstants);
			}

			if (flags & DynamicStateFlags::DEPTH_BOUNDS)
			{
				DynamicStateContainer::DepthBounds& bounds = pDynamicStates->mDepthBounds;
				vkCmdSetDepthBounds(GetCurrentCommandBuffer(), bounds.mBounds.mWidth, bounds.mBounds.mHeight);
			}

			for (UI8 i = 0; i < 10; i++)
			{
				if (!pDynamicStates->mConstantBlocks[i].IsNull())
				{
					DynamicStateContainer::ConstantData& data = pDynamicStates->mConstantBlocks[i];
					vkCmdPushConstants(GetCurrentCommandBuffer(), pPipeline->StaticCast<VulkanGraphicsPipeline>().GetPipelineLayout(), Utilities::GetShaderStage(ShaderType(i + 1)), static_cast<UI32>(data.mOffset), static_cast<UI32>(data.mSize), data.pData);
				}
			}
		}

		void VulkanCommandBufferList::IssueDrawCall(UI64 vertexOffset, UI64 vertexCount, UI64 indexOffset, UI64 indexCount)
		{
			FLINT_SETUP_PROFILER();

			vkCmdDrawIndexed(GetCurrentCommandBuffer(), static_cast<UI32>(indexCount), 1, static_cast<UI32>(indexOffset), static_cast<UI32>(vertexOffset), 0);
		}

		void VulkanCommandBufferList::ExecuteSecondaryCommands()
		{
			FLINT_SETUP_PROFILER();

			if (!vSecondaryCommandBuffers.empty())
				vkCmdExecuteCommands(GetCurrentCommandBuffer(), static_cast<UI32>(vSecondaryCommandBuffers.size()), vSecondaryCommandBuffers.data());
		}

		void VulkanCommandBufferList::EndBufferRecording()
		{
			FLINT_SETUP_PROFILER();

			FLINT_VK_ASSERT(vkEndCommandBuffer(vCurrentBuffer));
			bIsRecording = false;
		}

		void VulkanCommandBufferList::Terminate()
		{
			auto vDevice = pDevice->StaticCast<VulkanDevice>();
			vkFreeCommandBuffers(vDevice.GetLogicalDevice(), vCommandPool, static_cast<UI32>(vCommandBuffers.size()), vCommandBuffers.data());
			vkDestroyCommandPool(vDevice.GetLogicalDevice(), vCommandPool, nullptr);
		}

		void VulkanCommandBufferList::ClearBuffers()
		{
			for (VkCommandBuffer vBuffer : vCommandBuffers)
				vkResetCommandBuffer(vBuffer, 0);
		}

		VkCommandBufferInheritanceInfo VulkanCommandBufferList::GetInheritanceInfo() const
		{
			return VkCommandBufferInheritanceInfo();
		}

		void VulkanCommandBufferList::SetSecondaryCommandBuffers(std::vector<VkCommandBuffer>&& vSecondaryCommandBuffers)
		{
			this->vSecondaryCommandBuffers = std::move(vSecondaryCommandBuffers);
		}

		void VulkanCommandBufferList::VulkanBeginSecondaryCommandBuffer(UI32 bufferIndex, const VkCommandBufferInheritanceInfo* pInheritanceInfo)
		{
			FLINT_SETUP_PROFILER();

			mCurrentBufferIndex = bufferIndex;
			vCurrentBuffer = vCommandBuffers[bufferIndex];

			VkCommandBufferBeginInfo vBeginInfo = {};
			vBeginInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			vBeginInfo.pNext = VK_NULL_HANDLE;
			vBeginInfo.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT | VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
			vBeginInfo.pInheritanceInfo = pInheritanceInfo;

			FLINT_VK_ASSERT(vkBeginCommandBuffer(vCurrentBuffer, &vBeginInfo));
		}

		void VulkanCommandBufferList::VulkanBeginNextSecondaryCommandBuffer(const VkCommandBufferInheritanceInfo* pInheritanceInfo)
		{
			mCurrentBufferIndex++;
			if (mCurrentBufferIndex >= mBufferCount) mCurrentBufferIndex = 0;

			VulkanBeginSecondaryCommandBuffer(mCurrentBufferIndex, pInheritanceInfo);
		}
	}
}