// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/RenderTargets/VulkanScreenBoundRenderTargetS.h"
#include "VulkanBackend/RenderTargets/Pipelines/VulkanGraphicsPipeline.h"

namespace Flint
{
	namespace VulkanBackend
	{
		void VulkanScreenBoundRenderTargetS::Initialize(Backend::Device* pDevice, const Vector2& extent, UI32 bufferCount)
		{
			this->pDevice = pDevice;
			this->mExtent = extent;

			VulkanDevice* pvDevice = pDevice->Derive<VulkanDevice>();

			this->mBufferCount = pvDevice->FindSupporterBufferCount(bufferCount);

			vSwapChain.Initialize(pvDevice, extent, mBufferCount);
			vColorBuffer.Initialize(pvDevice, extent, vSwapChain.GetFormat(), mBufferCount);
			vDepthBuffer.Initialize(pvDevice, extent, mBufferCount);

			CreateRenderPass(pvDevice, { &vSwapChain, &vColorBuffer, &vDepthBuffer }, VK_PIPELINE_BIND_POINT_GRAPHICS);
			CreateFrameBuffer(pvDevice, { &vSwapChain, &vColorBuffer, &vDepthBuffer }, mExtent);
		}

		void VulkanScreenBoundRenderTargetS::Terminate()
		{
			vSwapChain.Terminate();
			vColorBuffer.Terminate();
			vDepthBuffer.Terminate();

			VulkanDevice* pvDevice = pDevice->Derive<VulkanDevice>();
			DestroyRenderPass(pvDevice);
			DestroyFrameBuffers(pvDevice);
		}

		Backend::GraphicsPipeline* VulkanScreenBoundRenderTargetS::CreateGraphicsPipeline(const std::vector<ShaderDigest>& shaderDigests, const Backend::GraphicsPipelineSpecification& spec)
		{
			VulkanGraphicsPipeline* pPipeline = new VulkanGraphicsPipeline();
			pPipeline->Initialize(this, shaderDigests, spec);
			return pPipeline;
		}

		void VulkanScreenBoundRenderTargetS::PrepareComputeResources()
		{
			VkComputePipelineCreateInfo vCI = {};
			vCI.sType = VK_STRUCTURE_TYPE_COMPUTE_PIPELINE_CREATE_INFO;
			vCI.flags = VK_NULL_HANDLE;
			vCI.pNext = VK_NULL_HANDLE;
			vCI.layout = vComputePipelineLayout;
		}

		void VulkanScreenBoundRenderTargetS::PrepareRenderResources()
		{
		}
	}
}