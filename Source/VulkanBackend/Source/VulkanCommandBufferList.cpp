// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend\VulkanCommandBufferList.h"
#include "VulkanBackend\VulkanMacros.h"
#include "VulkanBackend\RenderTargets\VulkanScreenBoundRenderTargetS.h"
#include "VulkanBackend\RenderTargets\Pipelines\VulkanGraphicsPipeline.h"

namespace Flint
{
	namespace VulkanBackend
	{
		void VulkanCommandBufferList::Initialize(DeviceType* pDevice, UI64 bufferCount)
		{
			this->pDevice = pDevice;
			this->mBufferCount = bufferCount;

			VkCommandPoolCreateInfo vCI = {};
			vCI.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
			vCI.pNext = VK_NULL_HANDLE;
			vCI.flags = VkCommandPoolCreateFlagBits::VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
			vCI.queueFamilyIndex = pDevice->GetQueue().mTransferFamily.value();

			FLINT_VK_ASSERT(pDevice->CreateCommanPool(&vCI, &vCommandPool), "Failed to create command pool!")

				VkCommandBufferAllocateInfo vAI = {};
			vAI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			vAI.pNext = VK_NULL_HANDLE;
			vAI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			vAI.commandPool = vCommandPool;
			vAI.commandBufferCount = static_cast<UI32>(GetBufferCount());

			vBuffers.resize(mBufferCount);
			FLINT_VK_ASSERT(pDevice->AllocateCommandBuffers(&vAI, vBuffers), "Failed to allocate command buffer!")
		}

		void VulkanCommandBufferList::Terminate()
		{
			pDevice->FreeComandBuffers(vCommandPool, vBuffers);
			pDevice->DestroyCommandPool(vCommandPool);
		}

		void VulkanCommandBufferList::ClearBuffers()
		{
			for (auto vBuffer : vBuffers)
				vkResetCommandBuffer(vBuffer, 0);
		}

		void VulkanCommandBufferList::ReceateBuffers()
		{
			pDevice->FreeComandBuffers(vCommandPool, vBuffers);

			VkCommandBufferAllocateInfo vAI = {};
			vAI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
			vAI.pNext = VK_NULL_HANDLE;
			vAI.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
			vAI.commandPool = vCommandPool;
			vAI.commandBufferCount = static_cast<UI32>(GetBufferCount());

			vBuffers.resize(mBufferCount);
			FLINT_VK_ASSERT(pDevice->AllocateCommandBuffers(&vAI, vBuffers), "Failed to allocate command buffer!")
		}

		void VulkanCommandBufferList::BeginBuffer(UI64 index)
		{
			mBufferIndex = index;

			VkCommandBufferBeginInfo vBI = {};
			vBI.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
			vBI.pNext = VK_NULL_HANDLE;
			vBI.pInheritanceInfo = VK_NULL_HANDLE;
			vBI.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;

			FLINT_VK_ASSERT(vkBeginCommandBuffer(vBuffers[GetIndex()], &vBI), "Failed to begin command buffer recording!")
		}

		void VulkanCommandBufferList::EndBuffer()
		{
			FLINT_VK_ASSERT(vkEndCommandBuffer(vBuffers[GetIndex()]), "Failed to end command buffer recording!")
		}

		void VulkanCommandBufferList::BindVertexBuffer(const BufferType& buffer, UI64 firstBinding, UI64 bindingCount)
		{
#ifdef FLINT_DEBUG
			if (buffer.GetUsage() != Backend::BufferUsage::VERTEX)
			{
				FLINT_LOG_ERROR(TEXT("Buffer type is not vertex!"))
					return;
			}
#endif // DEBUG

			VkDeviceSize pOffset[1] = { 0 };
			vkCmdBindVertexBuffers(GetCurrentBuffer(), static_cast<UI32>(firstBinding), static_cast<UI32>(bindingCount), &buffer.vBuffer, pOffset);
		}

		void VulkanCommandBufferList::BindIndexBuffer(const BufferType& buffer)
		{
			vkCmdBindIndexBuffer(GetCurrentBuffer(), buffer.vBuffer, 0, VK_INDEX_TYPE_UINT32);
		}

		void VulkanCommandBufferList::BindRenderTarget(const ScreenBoundRenderTargetType& renderTarget)
		{
			VkClearValue pClearValues[2] = {};
			pClearValues[0].color.float32[0] = CREATE_COLOR_256(32.0f);
			pClearValues[0].color.float32[1] = CREATE_COLOR_256(32.0f);
			pClearValues[0].color.float32[2] = CREATE_COLOR_256(32.0f);
			pClearValues[0].color.float32[3] = 1.0f;
			pClearValues[1].depthStencil = { 1.0f, 0 };

			VkRenderPassBeginInfo vRBI = {};
			vRBI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			vRBI.pNext = VK_NULL_HANDLE;
			vRBI.clearValueCount = 2;
			vRBI.pClearValues = pClearValues;
			vRBI.renderPass = renderTarget.GetRenderPass();
			vRBI.framebuffer = renderTarget.GetCurrentFrameBuffer();
			vRBI.renderArea.extent = { static_cast<UI32>(renderTarget.GetExtent().x), static_cast<UI32>(renderTarget.GetExtent().y) };

			vkCmdBeginRenderPass(GetCurrentBuffer(), &vRBI, VK_SUBPASS_CONTENTS_INLINE);
		}

		void VulkanCommandBufferList::EndRenderTarget()
		{
			vkCmdEndRenderPass(GetCurrentBuffer());
		}

		void VulkanCommandBufferList::BindPipeline(const ScreenBoundGraphicsPipelineType& graphicsPipeline)
		{
			vkCmdBindPipeline(GetCurrentBuffer(), graphicsPipeline.GetBindPoint(), graphicsPipeline.vPipeline);
		}

		void VulkanCommandBufferList::SetDynamicStates(const Backend::DynamicStateContainer& container)
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

					vkCmdSetViewport(GetCurrentBuffer(), 0, 1, &vVP);
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

					vkCmdSetScissor(GetCurrentBuffer(), 0, 1, &vR2D);
				}
				break;

				case Flint::Backend::DynamicStateFlags::LINE_WIDTH:
				{
					Backend::DynamicStateContainer::LineWidth* pLineWidth = dynamic_cast<Backend::DynamicStateContainer::LineWidth*>(pState.get());

					vkCmdSetLineWidth(GetCurrentBuffer(), pLineWidth->mLineWidth);
				}
				break;

				case Flint::Backend::DynamicStateFlags::DEPTH_BIAS:
				{
					Backend::DynamicStateContainer::DepthBias* pDepthBias = dynamic_cast<Backend::DynamicStateContainer::DepthBias*>(pState.get());

					vkCmdSetDepthBias(GetCurrentBuffer(), pDepthBias->mDepthBiasFactor, pDepthBias->mDepthClampFactor, pDepthBias->mDepthSlopeFactor);
				}
				break;

				case Flint::Backend::DynamicStateFlags::BLEND_CONSTANTS:
				{
					Backend::DynamicStateContainer::BlendConstants* pBlendConstants = dynamic_cast<Backend::DynamicStateContainer::BlendConstants*>(pState.get());

					vkCmdSetBlendConstants(GetCurrentBuffer(), pBlendConstants->mConstants);
				}
				break;

				case Flint::Backend::DynamicStateFlags::DEPTH_BOUNDS:
				{
					Backend::DynamicStateContainer::DepthBounds* pDepthBounds = dynamic_cast<Backend::DynamicStateContainer::DepthBounds*>(pState.get());

					vkCmdSetDepthBounds(GetCurrentBuffer(), pDepthBounds->mBounds.x, pDepthBounds->mBounds.y);
				}
				break;

				default:
					FLINT_LOG_ERROR(TEXT("Invalid or Undefined dynamic state flag!"))
						break;
				}
			}
		}
		
		void VulkanCommandBufferList::BindRenderResource(const ScreenBoundGraphicsPipelineResourceType& resource)
		{
			vkCmdBindDescriptorSets(GetCurrentBuffer(), resource.GetPipeline()->GetBindPoint(), resource.GetPipeline()->vPipelineLayout, 0, 1, &resource.vDescriptorSet, 0, nullptr);
		}
		
		void VulkanCommandBufferList::DrawIndexed(UI64 indexCount, UI64 indexOffset, UI64 vertexOffset)
		{
			vkCmdDrawIndexed(GetCurrentBuffer(), static_cast<UI32>(indexCount), 1, static_cast<UI32>(indexOffset), static_cast<UI32>(vertexOffset), 0);
		}
	}
}
