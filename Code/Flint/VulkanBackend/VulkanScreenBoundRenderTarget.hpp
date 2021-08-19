// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ScreenBoundRenderTarget.hpp"
#include "Thread/CommandCenter.hpp"

#include "VulkanRenderTarget.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanColorBuffer.hpp"
#include "VulkanDepthBuffer.hpp"

#include "Core/BinarySemaphore.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanCommandBufferList;

		class VulkanScreenBoundRenderTarget final : public ScreenBoundRenderTarget, public std::enable_shared_from_this<VulkanScreenBoundRenderTarget>
		{
		public:
			VulkanScreenBoundRenderTarget(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Display>& pDisplay, const FBox2D& extent, const UI32 bufferCount, UI32 threadCount = 0);

			virtual void PrepareStaticResources() override final;
			virtual void BeginFrame() override final;
			virtual void Update() override final;
			virtual void SubmitFrame() override final;
			virtual void Terminate() override final;

			virtual void Recreate() override final;

			virtual FColor4D GetClearColor() const override final;
			virtual void SetClearColor(const FColor4D& newColor) override final;

			VkRenderPass GetRenderPass() const { return vRenderTarget.vRenderPass; }
			VkFramebuffer GetFrameBuffer(UI32 index) const { return vRenderTarget.vFrameBuffers[index]; }

			UI32 GetClearScreenValueCount() const { return  2; }
			const VkClearValue* GetClearScreenValues() const { return pClearValues; }

			const VkCommandBufferInheritanceInfo* GetVulkanInheritanceInfo() const { return &vInheritInfo; }
			void AddVulkanCommandBuffer(VkCommandBuffer vCommandBuffer) { vSecondaryCommandBuffers.push_back(vCommandBuffer); }
			void AddSubmitCommandBuffer(VkCommandBuffer vCommandBuffer) { vSubmitCommandBuffers.push_back(vCommandBuffer); }

		private:
			void BindSecondaryCommands();

			virtual void BindVolatileInstances() override final;
			virtual void SecondaryCommandsWorker(DrawInstanceMap& drawInstanceMap, std::list<std::shared_ptr<GeometryStore>>& drawOrder, BinarySemaphore& binarySemaphore, CountingSemaphore& countingSemaphore, std::atomic<bool>& shouldRun) override final;
			void ExecuteSecondaryCommandBuffers();

		private:
			VulkanRenderTarget vRenderTarget;

			std::atomic<VkCommandBufferInheritanceInfo> vInheritanceInfo = {};
			VkCommandBufferInheritanceInfo vInheritInfo = {};
			std::vector<VkCommandBuffer> vSecondaryCommandBuffers = {};
			std::vector<VkCommandBuffer> vSubmitCommandBuffers = {};

			VkSubmitInfo vSI = {};
			VkPresentInfoKHR vPI = {};

			std::mutex mResourceMutex = {};

			std::unique_ptr<VulkanSwapChain> pSwapChain = nullptr;
			std::unique_ptr<VulkanColorBuffer> pColorBuffer = nullptr;
			std::unique_ptr<VulkanDepthBuffer> pDepthBuffer = nullptr;
			std::unique_ptr<VulkanCommandBufferList> pDefaultSecondaryCommandBuffer = nullptr;

			bool bShouldSkip = false;

			VkClearValue pClearValues[2] = {};
			std::shared_ptr<ScreenBoundRenderTarget> pThisRenderTarget = nullptr;

			VkCommandBuffer vCommandBuffer[1] = {};
			VkSemaphoreWaitFlags vWaitStage = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		};
	}
}