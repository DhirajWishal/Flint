// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/OffScreenRenderTargets/VulkanSAVSM.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanSAVSM::VulkanSAVSM(const std::shared_ptr<Device>& pDevice, const FBox2D& extent, const UI32 bufferCount, UI32 threadCount)
			: VulkanOffScreenRenderTarget(pDevice, OffScreenRenderTargetType::SUMMED_AREA_VARIENT_SHADOW_MAP, extent, bufferCount, threadCount)
		{
		}

		void VulkanSAVSM::Execute(const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget)
		{
		}

		void VulkanSAVSM::Terminate()
		{
		}

		VkFramebuffer VulkanSAVSM::GetFrameBuffer(UI32 index) const
		{
			return VkFramebuffer();
		}

		FColor4D VulkanSAVSM::GetClearColor() const
		{
			return FColor4D();
		}

		void VulkanSAVSM::SetClearColor(const FColor4D& newColor)
		{
		}
	}
}