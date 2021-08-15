// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanOffScreenRenderTargetFactory.hpp"
#include "VulkanBackend/OffScreenRenderTargets/VulkanShadowMap.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		std::shared_ptr<OffScreenRenderTarget> VulkanOffScreenRenderTargetFactory::Create(OffScreenRenderTargetType type, const FBox2D& extent, UI32 bufferCount, UI32 threadCount)
		{
			switch (type)
			{
			case Flint::OffScreenRenderTargetType::SHADOW_MAP:
				return std::make_shared<VulkanShadowMap>(pDevice, extent, bufferCount, threadCount);

			case Flint::OffScreenRenderTargetType::POINT_SHADOW_MAP:
				break;

			default:
				FLINT_THROW_INVALID_ARGUMENT("Invalid off screen render target type!");
			}

			return nullptr;
		}
	}
}