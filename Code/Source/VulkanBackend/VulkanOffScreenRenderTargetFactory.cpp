// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanOffScreenRenderTargetFactory.hpp"
#include "VulkanBackend/OffScreenRenderTargets/VulkanShadowMap.hpp"
#include "VulkanBackend/OffScreenRenderTargets/VulkanPointShadowMap.hpp"
#include "VulkanBackend/OffScreenRenderTargets/VulkanSAVSM.hpp"
#include "VulkanBackend/OffScreenRenderTargets/VulkanMousePicker.hpp"

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
				return std::make_shared<VulkanPointShadowMap>(pDevice, extent, bufferCount, threadCount);

			case Flint::OffScreenRenderTargetType::SUMMED_AREA_VARIENT_SHADOW_MAP:
				return std::make_shared<VulkanSAVSM>(pDevice, extent, bufferCount, threadCount);

			case Flint::OffScreenRenderTargetType::MOUSE_PICKER:
				return std::make_shared<VulkanMousePicker>(pDevice, extent, bufferCount, threadCount);

			default:
				FLINT_THROW_INVALID_ARGUMENT("Invalid off screen render target type!");
			}

			return nullptr;
		}
	}
}