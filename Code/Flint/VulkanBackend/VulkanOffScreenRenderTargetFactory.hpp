// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "OffScreenRenderTargetFactory.hpp"
#include "VulkanDevice.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		class VulkanOffScreenRenderTargetFactory final : public OffScreenRenderTargetFactory
		{
		public:
			VulkanOffScreenRenderTargetFactory(const std::shared_ptr<Device>& pDevice) : OffScreenRenderTargetFactory(pDevice) {}

			virtual std::shared_ptr<OffScreenRenderTarget> Create(OffScreenRenderTargetType type, const FBox2D& extent, UI32 bufferCount, UI32 threadCount = 0) override final;
		};
	}
}