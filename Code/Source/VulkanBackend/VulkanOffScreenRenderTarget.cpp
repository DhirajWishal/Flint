// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanOffScreenRenderTarget.hpp"
#include "VulkanBackend/VulkanScreenBoundRenderTarget.hpp"
#include "VulkanBackend/VulkanCommandBufferList.hpp"
#include "VulkanBackend/VulkanGraphicsPipeline.hpp"
#include "VulkanBackend/VulkanImage.hpp"

#include "GeometryStore.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		namespace _Helpers
		{
			PixelFormat GetPixelFormat(VkFormat format)
			{
				switch (format)
				{
				case VK_FORMAT_UNDEFINED:
					return PixelFormat::UNDEFINED;

				case VK_FORMAT_R8_SRGB:
					return PixelFormat::R8_SRGB;

				case VK_FORMAT_R8G8_SRGB:
					return PixelFormat::R8G8_SRGB;

				case VK_FORMAT_R8G8B8_SRGB:
					return PixelFormat::R8G8B8_SRGB;

				case VK_FORMAT_R8G8B8A8_SRGB:
					return PixelFormat::R8G8B8A8_SRGB;

				case VK_FORMAT_B8G8R8_SRGB:
					return PixelFormat::B8G8R8_SRGB;

				case VK_FORMAT_B8G8R8A8_SRGB:
					return PixelFormat::B8G8R8A8_SRGB;

				case VK_FORMAT_D16_UNORM:
					return PixelFormat::D16_SINT;

				case VK_FORMAT_D32_SFLOAT:
					return PixelFormat::D32_SFLOAT;

				default:
					FLINT_THROW_BACKEND_ERROR("Unsupported format!");
				}

				return PixelFormat::UNDEFINED;
			}
		}

		VulkanOffScreenRenderTarget::VulkanOffScreenRenderTarget(const std::shared_ptr<Device>& pDevice, OffScreenRenderTargetType type, const FBox2D& extent, const UI32 bufferCount, UI32 threadCount)
			: OffScreenRenderTarget(pDevice, type, extent, bufferCount, pDevice->CreatePrimaryCommandBufferList(bufferCount), threadCount), vRenderTarget(pDevice->StaticCast<VulkanDevice>())
		{
			vInheritInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_COMMAND_BUFFER_INHERITANCE_INFO;

			// Setup default clear color values.
			pClearValues[1].color.float32[0] = CREATE_COLOR_256(32.0f);
			pClearValues[1].color.float32[1] = CREATE_COLOR_256(32.0f);
			pClearValues[1].color.float32[2] = CREATE_COLOR_256(32.0f);
			pClearValues[1].color.float32[3] = 1.0f;
			pClearValues[0].depthStencil.depth = 1.0f;
			pClearValues[0].depthStencil.stencil = 0;
		}

		void VulkanOffScreenRenderTarget::BindVolatileInstances()
		{
		}

		void VulkanOffScreenRenderTarget::SecondaryCommandsWorker(DrawInstanceMap& drawInstanceMap, std::list<std::shared_ptr<GeometryStore>>& drawOrder, BinarySemaphore& binarySemaphore, CountingSemaphore& countingSemaphore, std::atomic<bool>& shouldRun)
		{
		}

		FColor4D VulkanOffScreenRenderTarget::GetClearColor() const
		{
			return FColor4D(pClearValues[0].color.float32[0], pClearValues[0].color.float32[1], pClearValues[0].color.float32[2], pClearValues[0].color.float32[3]);
		}

		void VulkanOffScreenRenderTarget::SetClearColor(const FColor4D& newColor)
		{
			pClearValues[0].color.float32[0] = newColor.mRed;
			pClearValues[0].color.float32[1] = newColor.mGreen;
			pClearValues[0].color.float32[2] = newColor.mBlue;
			pClearValues[0].color.float32[3] = newColor.mAlpha;
		}
	}
}

/*
vkCmdExecuteCommands() called w/ invalid secondary VkCommandBuffer 0x1fac70410b8[] which has a VkFramebuffer 0xf37618000000003d[] that is not the same as the primary command buffer's current active VkFramebuffer 0xc1547b000000003e[]. The Vulkan spec states: If vkCmdExecuteCommands is being called within a render pass instance, and any element of pCommandBuffers was recorded with VkCommandBufferInheritanceInfo::framebuffer not equal to VK_NULL_HANDLE, that VkFramebuffer must match the VkFramebuffer used in the current render pass instance (https://vulkan.lunarg.com/doc/view/1.2.162.1/windows/1.2-extensions/vkspec.html#VUID-vkCmdExecuteCommands-pCommandBuffers-00099)
Calling vkBeginCommandBuffer() on active VkCommandBuffer 0x1fac70410b8[] before it has completed. You must check command buffer fence before this call. The Vulkan spec states: commandBuffer must not be in the recording or pending state (https://vulkan.lunarg.com/doc/view/1.2.162.1/windows/1.2-extensions/vkspec.html#VUID-vkBeginCommandBuffer-commandBuffer-00049)

vkCmdExecuteCommands() called w/ invalid secondary VkCommandBuffer 0x1fac70410b8[] which has a VkFramebuffer 0xf37618000000003d[] that is not the same as the primary command buffer's current active VkFramebuffer 0xc1547b000000003e[]. The Vulkan spec states: If vkCmdExecuteCommands is being called within a render pass instance, and any element of pCommandBuffers was recorded with VkCommandBufferInheritanceInfo::framebuffer not equal to VK_NULL_HANDLE, that VkFramebuffer must match the VkFramebuffer used in the current render pass instance (https://vulkan.lunarg.com/doc/view/1.2.162.1/windows/1.2-extensions/vkspec.html#VUID-vkCmdExecuteCommands-pCommandBuffers-00099)
Calling vkBeginCommandBuffer() on active VkCommandBuffer 0x1fac70410b8[] before it has completed. You must check command buffer fence before this call. The Vulkan spec states: commandBuffer must not be in the recording or pending state (https://vulkan.lunarg.com/doc/view/1.2.162.1/windows/1.2-extensions/vkspec.html#VUID-vkBeginCommandBuffer-commandBuffer-00049)

vkCmdExecuteCommands() called w/ invalid secondary VkCommandBuffer 0x1fac70410b8[] which has a VkFramebuffer 0xf37618000000003d[] that is not the same as the primary command buffer's current active VkFramebuffer 0xd20a5a000000003f[]. The Vulkan spec states: If vkCmdExecuteCommands is being called within a render pass instance, and any element of pCommandBuffers was recorded with VkCommandBufferInheritanceInfo::framebuffer not equal to VK_NULL_HANDLE, that VkFramebuffer must match the VkFramebuffer used in the current render pass instance (https://vulkan.lunarg.com/doc/view/1.2.162.1/windows/1.2-extensions/vkspec.html#VUID-vkCmdExecuteCommands-pCommandBuffers-00099)
Calling vkBeginCommandBuffer() on active VkCommandBuffer 0x1fac70410b8[] before it has completed. You must check command buffer fence before this call. The Vulkan spec states: commandBuffer must not be in the recording or pending state (https://vulkan.lunarg.com/doc/view/1.2.162.1/windows/1.2-extensions/vkspec.html#VUID-vkBeginCommandBuffer-commandBuffer-00049)

vkCmdExecuteCommands() called w/ invalid secondary VkCommandBuffer 0x1fac70410b8[] which has a VkFramebuffer 0xf37618000000003d[] that is not the same as the primary command buffer's current active VkFramebuffer 0xd20a5a000000003f[]. The Vulkan spec states: If vkCmdExecuteCommands is being called within a render pass instance, and any element of pCommandBuffers was recorded with VkCommandBufferInheritanceInfo::framebuffer not equal to VK_NULL_HANDLE, that VkFramebuffer must match the VkFramebuffer used in the current render pass instance (https://vulkan.lunarg.com/doc/view/1.2.162.1/windows/1.2-extensions/vkspec.html#VUID-vkCmdExecuteCommands-pCommandBuffers-00099)
Calling vkBeginCommandBuffer() on active VkCommandBuffer 0x1fac70410b8[] before it has completed. You must check command buffer fence before this call. The Vulkan spec states: commandBuffer must not be in the recording or pending state (https://vulkan.lunarg.com/doc/view/1.2.162.1/windows/1.2-extensions/vkspec.html#VUID-vkBeginCommandBuffer-commandBuffer-00049)
*/