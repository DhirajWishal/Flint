// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ScreenBoundRenderTarget.hpp"
#include "Thread/CommandCenter.hpp"

#include "VulkanRenderTarget.hpp"
#include "VulkanSwapChain.hpp"
#include "VulkanColorBuffer.hpp"
#include "VulkanDepthBuffer.hpp"

#include <semaphore>

namespace Flint
{
	namespace VulkanBackend
	{
		struct RecordCommandBuffer final : public Thread::CommandBase
		{
			RecordCommandBuffer(VkCommandBufferInheritanceInfo* pInheritanceInfo) : CommandBase(0), pInheritanceInfo(pInheritanceInfo) {}

			VkCommandBufferInheritanceInfo* pInheritanceInfo = nullptr;
		};

		struct TerminateWorker final : public Thread::CommandBase
		{
			TerminateWorker() : CommandBase(1) {}
		};

		class VulkanScreenBoundRenderTarget final : public ScreenBoundRenderTarget, public std::enable_shared_from_this<VulkanScreenBoundRenderTarget>
		{
		public:
			VulkanScreenBoundRenderTarget(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Display>& pDisplay, const FBox2D& extent, const UI32 bufferCount);

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
			void SecondaryCommandWorker();

		private:
			VulkanRenderTarget vRenderTarget;

			VkCommandBufferInheritanceInfo vInheritanceInfo = {};

			std::atomic<std::shared_ptr<Thread::CommandCenter>> pCommandCenter;
			std::thread mWorkerThread;

			VulkanSwapChain* pSwapChain = nullptr;
			VulkanColorBuffer* pColorBuffer = nullptr;
			VulkanDepthBuffer* pDepthBuffer = nullptr;

			bool bShouldSkip = false;

			VkClearValue pClearValues[2] = {};
			std::shared_ptr<ScreenBoundRenderTarget> pThisRenderTarget = nullptr;
			std::binary_semaphore mRenderTargetToThreadSemaphore{ 0 }, mThreadToRenderTargetSemaphore{ 0 };
		};
	}
}