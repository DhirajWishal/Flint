// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/RenderTargets/VulkanScreenBoundRenderTargetS.h"
#include "VulkanBackend/RenderTargets/Pipelines/VulkanGraphicsPipeline.h"
#include "VulkanBackend/VulkanCommandBufferManager.h"

namespace Flint
{
	namespace VulkanBackend
	{
		void VulkanScreenBoundRenderTargetS::Initialize(Backend::Device* pDevice, const Vector2& extent, UI32 bufferCount)
		{
			this->pDevice = pDevice;
			this->mExtent = extent;
			this->pCommandBufferManager = new VulkanCommandBufferManager();

			VulkanDevice* pvDevice = pDevice->Derive<VulkanDevice>();
			this->mBufferCount = pvDevice->FindSupporterBufferCount(bufferCount);

			pCommandBufferManager->CreateBuffers(pDevice, static_cast<UI32>(mBufferCount));
			vSwapChain.Initialize(pvDevice, extent, static_cast<UI32>(mBufferCount));
			vColorBuffer.Initialize(pvDevice, extent, vSwapChain.GetFormat(), static_cast<UI32>(mBufferCount));
			vDepthBuffer.Initialize(pvDevice, extent, static_cast<UI32>(mBufferCount));

			CreateRenderPass(pvDevice, { &vSwapChain, &vColorBuffer, &vDepthBuffer }, VK_PIPELINE_BIND_POINT_GRAPHICS);
			CreateFrameBuffer(pvDevice, { &vSwapChain, &vColorBuffer, &vDepthBuffer }, mExtent, static_cast<UI32>(mBufferCount));
		}

		void VulkanScreenBoundRenderTargetS::Terminate()
		{
			vSwapChain.Terminate();
			vColorBuffer.Terminate();
			vDepthBuffer.Terminate();

			VulkanDevice* pvDevice = pDevice->Derive<VulkanDevice>();
			DestroyRenderPass(pvDevice);
			DestroyFrameBuffers(pvDevice);

			pCommandBufferManager->Terminate();
			delete pCommandBufferManager;
		}

		Backend::GraphicsPipeline* VulkanScreenBoundRenderTargetS::CreateGraphicsPipeline(const std::vector<ShaderDigest>& shaderDigests, const Backend::GraphicsPipelineSpecification& spec)
		{
			VulkanGraphicsPipeline* pPipeline = new VulkanGraphicsPipeline();
			pPipeline->Initialize(this, shaderDigests, spec);
			return pPipeline;
		}

		void VulkanScreenBoundRenderTargetS::Bind(Backend::CommandBuffer commandBuffer)
		{
			VkClearValue pClearValues[2] = {};
			pClearValues[0].depthStencil = { 1.0f, 0 };
			pClearValues[1].color.float32[0] = (64.0f / 255.0f);
			pClearValues[1].color.float32[1] = (64.0f / 255.0f);
			pClearValues[1].color.float32[2] = (64.0f / 255.0f);
			pClearValues[1].color.float32[3] = 1.0f;

			VkRenderPassBeginInfo vRBI = {};
			vRBI.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
			vRBI.pNext = VK_NULL_HANDLE;
			vRBI.clearValueCount = 2;
			vRBI.pClearValues = pClearValues;
			vRBI.renderPass = vRenderPass;
			vRBI.framebuffer = vFrameBuffers[commandBuffer.GetIndex()];
			vRBI.renderArea.extent = { static_cast<UI32>(mExtent.x), static_cast<UI32>(mExtent.y) };

			VkCommandBuffer vCommandBuffer = commandBuffer.GetManager()->Derive<VulkanCommandBufferManager>()->vCommandBuffers[commandBuffer.GetIndex()];
			vkCmdBeginRenderPass(vCommandBuffer, &vRBI, VK_SUBPASS_CONTENTS_INLINE);
		}

		void VulkanScreenBoundRenderTargetS::UnBind(Backend::CommandBuffer commandBuffer)
		{
			VkCommandBuffer vCommandBuffer = commandBuffer.GetManager()->Derive<VulkanCommandBufferManager>()->vCommandBuffers[commandBuffer.GetIndex()];
			vkCmdEndRenderPass(vCommandBuffer);
		}
	}
}