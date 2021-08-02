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

		private:
			void BindSecondaryCommands();
			virtual void SecondaryCommandsWorker(DrawInstanceMap& drawInstanceMap, BinarySemaphore& binarySemaphore, CountingSemaphore& countingSemaphore, std::atomic<bool>& shouldRun) override final;
			void ExecuteSecondaryCommandBuffers();

		private:
			VulkanRenderTarget vRenderTarget;

			std::atomic<VkCommandBufferInheritanceInfo> vInheritanceInfo = {};
			std::vector<VkCommandBuffer> vSecondaryCommandBuffers = {};

			std::mutex mResourceMutex = {};

			VulkanSwapChain* pSwapChain = nullptr;
			VulkanColorBuffer* pColorBuffer = nullptr;
			VulkanDepthBuffer* pDepthBuffer = nullptr;

			bool bShouldSkip = false;

			VkClearValue pClearValues[2] = {};
			std::shared_ptr<ScreenBoundRenderTarget> pThisRenderTarget = nullptr;
		};
	}
}