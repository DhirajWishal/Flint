// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanBackend/VulkanOffScreenRenderTarget.hpp"
#include "VulkanBackend/VulkanRenderTarget.hpp"
#include "VulkanBackend/VulkanCommandBufferList.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanMousePicker final : public VulkanOffScreenRenderTarget
		{
		public:
			VulkanMousePicker(const std::shared_ptr<Device>& pDevice, const FBox2D& extent, const UI32 bufferCount, UI32 threadCount = 0);

			virtual void Execute(const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget = nullptr) override final;
			virtual void Terminate() override final;

			virtual VkFramebuffer GetFrameBuffer(UI32 index) const override final;

			virtual FColor4D GetClearColor() const override final;
			virtual void SetClearColor(const FColor4D& newColor) override final;

			virtual const UI32 GetClearScreenValueCount() const override final { return static_cast<UI32>(pResults.size()); }
			virtual const VkClearValue* GetClearScreenValues() const override final { return &vClearValue; }

		private:
			VkClearValue vClearValue = {};
		};
	}
}