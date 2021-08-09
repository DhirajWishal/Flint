// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "RenderTarget.hpp"
#include "Image.hpp"

namespace Flint
{
	class ScreenBoundRenderTarget;

	/**
	 * Off screen render target attachment enum.
	 */
	enum class OffScreenRenderTargetAttachment : UI8 {
		COLOR_BUFFER = BIT_SHIFT(0),
		DEPTH_BUFFER = BIT_SHIFT(1)
	};

	/**
	 * Bitwise OR operator for OffScreenRenderTargetAttachment.
	 *
	 * @param lhs: The left hand side argument.
	 * @param rhs: The right hand side argument.
	 * @return The OR-ed result.
	 */
	constexpr OffScreenRenderTargetAttachment operator|(const OffScreenRenderTargetAttachment& lhs, const OffScreenRenderTargetAttachment& rhs)
	{
		return static_cast<OffScreenRenderTargetAttachment>(static_cast<UI8>(lhs) | static_cast<UI8>(rhs));
	}

	/**
	 * Bitwise AND operator for OffScreenRenderTargetAttachment.
	 *
	 * @param lhs: The left hand side argument.
	 * @param rhs: The right hand side argument.
	 * @return The AND-ed result.
	 */
	constexpr OffScreenRenderTargetAttachment operator&(const OffScreenRenderTargetAttachment& lhs, const OffScreenRenderTargetAttachment& rhs)
	{
		return static_cast<OffScreenRenderTargetAttachment>(static_cast<UI8>(lhs) & static_cast<UI8>(rhs));
	}

	/**
	 * Flint off screen render target.
	 * Off screen render targets are used for computational and general off screen processes like shadow mapping.
	 */
	class OffScreenRenderTarget : public RenderTarget
	{
	public:
		/**
		 * Default constructor.
		 *
		 * @param pDevice: The device pointer.
		 * @param extent: The frame buffer extent.
		 * @param bufferCount: The frame buffer count.
		 * @param pCommandBufferList: The command buffer list to be used.
		 * @param attachments: The render target attachments. Default is OffScreenRenderTargetAttachment::COLOR_BUFFER | OffScreenRenderTargetAttachment::DEPTH_BUFFER.
		 * @param threadCount: The number of threads to use. Default is 0.
		 */
		OffScreenRenderTarget(
			const std::shared_ptr<Device>& pDevice,
			const FBox2D& extent,
			const UI32 bufferCount,
			const std::shared_ptr<CommandBufferList>& pCommandBufferList,
			OffScreenRenderTargetAttachment attachments = OffScreenRenderTargetAttachment::COLOR_BUFFER | OffScreenRenderTargetAttachment::DEPTH_BUFFER,
			UI32 threadCount = 0);

		virtual void Execute(const std::shared_ptr<ScreenBoundRenderTarget>& pScreenBoundRenderTarget = nullptr) = 0;

	protected:
		std::vector<std::shared_ptr<Image>> pResults = {};

		OffScreenRenderTargetAttachment mAttachments = OffScreenRenderTargetAttachment(0);
	};
}