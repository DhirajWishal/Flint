// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanCommandBuffer.h"
#include "VulkanBackend/VulkanDevice.h"
#include "VulkanBackend/VulkanMacros.h"
#include "VulkanBackend/RenderTargets/VulkanScreenBoundRenderTargetS.h"

namespace Flint
{
	namespace VulkanBackend
	{
		void VulkanCommandBuffer::BeginBufferRecording()
		{
			VkCommandBufferBeginInfo vBI = {};
			vBI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			vBI.pNext = VK_NULL_HANDLE;
			vBI.pInheritanceInfo = pInheritanceInfo;
			//vBI.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

			if (pInheritanceInfo)
				vBI.flags = VkCommandBufferUsageFlagBits::VK_COMMAND_BUFFER_USAGE_RENDER_PASS_CONTINUE_BIT;

			FLINT_VK_ASSERT(vkBeginCommandBuffer(vBuffer, &vBI), "Failed to begin command buffer recording!")
		}

		void VulkanCommandBuffer::EndBufferRecording()
		{
			FLINT_VK_ASSERT(vkEndCommandBuffer(vBuffer), "Failed to end command buffer recording!")
		}

		void VulkanCommandBuffer::SubmitDynamicStates(const Backend::DynamicStateContainer& container)
		{
			for (auto& pState : container.pDynamicStates)
			{
				switch (pState->GetFlag())
				{
				case Flint::Backend::DynamicStateFlags::VIEWPORT:
				{
					Backend::DynamicStateContainer::ViewPort* pViewPort = dynamic_cast<Backend::DynamicStateContainer::ViewPort*>(pState.get());

					VkViewport vVP = {};
					vVP.width = pViewPort->mExtent.width;
					vVP.height = pViewPort->mExtent.height;
					vVP.minDepth = pViewPort->mDepth.x;
					vVP.maxDepth = pViewPort->mDepth.y;
					vVP.x = pViewPort->mOffset.x;
					vVP.y = pViewPort->mOffset.y;

					vkCmdSetViewport(vBuffer, 0, 1, &vVP);
				}
				break;

				case Flint::Backend::DynamicStateFlags::SCISSOR:
				{
					Backend::DynamicStateContainer::Scissor* pScissor = dynamic_cast<Backend::DynamicStateContainer::Scissor*>(pState.get());

					VkRect2D vR2D = {};
					vR2D.extent.width = static_cast<UI32>(pScissor->mExtent.width);
					vR2D.extent.height = static_cast<UI32>(pScissor->mExtent.height);
					vR2D.offset.x = static_cast<I32>(pScissor->mOffset.x);
					vR2D.offset.y = static_cast<I32>(pScissor->mOffset.y);

					vkCmdSetScissor(vBuffer, 0, 1, &vR2D);
				}
				break;

				case Flint::Backend::DynamicStateFlags::LINE_WIDTH:
				{
					Backend::DynamicStateContainer::LineWidth* pLineWidth = dynamic_cast<Backend::DynamicStateContainer::LineWidth*>(pState.get());

					vkCmdSetLineWidth(vBuffer, pLineWidth->mLineWidth);
				}
				break;

				case Flint::Backend::DynamicStateFlags::DEPTH_BIAS:
				{
					Backend::DynamicStateContainer::DepthBias* pDepthBias = dynamic_cast<Backend::DynamicStateContainer::DepthBias*>(pState.get());

					vkCmdSetDepthBias(vBuffer, pDepthBias->mDepthBiasFactor, pDepthBias->mDepthClampFactor, pDepthBias->mDepthSlopeFactor);
				}
				break;

				case Flint::Backend::DynamicStateFlags::BLEND_CONSTANTS:
				{
					Backend::DynamicStateContainer::BlendConstants* pBlendConstants = dynamic_cast<Backend::DynamicStateContainer::BlendConstants*>(pState.get());

					vkCmdSetBlendConstants(vBuffer, pBlendConstants->mConstants);
				}
				break;

				case Flint::Backend::DynamicStateFlags::DEPTH_BOUNDS:
				{
					Backend::DynamicStateContainer::DepthBounds* pDepthBounds = dynamic_cast<Backend::DynamicStateContainer::DepthBounds*>(pState.get());

					vkCmdSetDepthBounds(vBuffer, pDepthBounds->mBounds.x, pDepthBounds->mBounds.y);
				}
				break;

				default:
					FLINT_LOG_ERROR(TEXT("Invalid or Undefined dynamic state flag!"))
						break;
				}
			}
		}

		void VulkanCommandBuffer::DrawUsingIndexData(UI32 indexCount, UI32 vertexOffset, UI32 indexOffset)
		{
			vkCmdDrawIndexed(vBuffer, indexCount, 1, indexOffset, vertexOffset, 0);
		}

		void VulkanCommandBuffer::ExecuteChildCommands(const std::vector<std::shared_ptr<Backend::CommandBuffer>>& pCommandBuffers)
		{
			std::vector<VkCommandBuffer> vCommandBuffers;
			for (auto& ptr : pCommandBuffers)
				INSERT_INTO_VECTOR(vCommandBuffers, *ptr->Derive<VulkanCommandBuffer>());

			vkCmdExecuteCommands(vBuffer, static_cast<UI32>(vCommandBuffers.size()), vCommandBuffers.data());
		}
	}
}