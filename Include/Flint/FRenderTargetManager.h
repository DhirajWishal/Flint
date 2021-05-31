// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FDeviceManager.h"
#include "Core/Global.h"

#include <memory>

namespace Flint
{
	DEFINE_HANDLE_UI16(FRenderTargetHandle);

	/**
	 * Render target attachment type enum.
	 */
	enum class RenderTargetAttachmentType : UI8 {
		SWAP_CHAIN = BIT_SHIFT(0),
		COLOR_BUFFER = BIT_SHIFT(1),
		DEPTH_BUFFER = BIT_SHIFT(2)
	};

	/**
	 * Flint render target attachment object.
	 * This is the base class for all the render target attachments and is used to describe the render target contents.
	 */
	class FLINT_API FRenderTargetAttachment
	{
	public:
		FRenderTargetAttachment(RenderTargetAttachmentType type) : mType(type) {}

		/**
		 * Get the type of the attachment.
		 *
		 * @return The attachment type.
		 */
		RenderTargetAttachmentType GetType() const { return mType; }

	protected:
		RenderTargetAttachmentType mType = RenderTargetAttachmentType::SWAP_CHAIN;
	};

	/**
	 * Flint swap chain attachment object.
	 * This object holds information related to the swap chain used by the render target.
	 *
	 * Swap chains use a display handle to render content to the display/ window.
	 */
	class FLINT_API FSwapChainAttachment final : public FRenderTargetAttachment
	{
	public:
		FSwapChainAttachment(FDisplayHandle displayHandle) : FRenderTargetAttachment(RenderTargetAttachmentType::SWAP_CHAIN), mDisplayHandle(displayHandle) {}

		/**
		 * Create a new swap chain attachment.
		 *
		 * @param displayHandle: The display handle which the swap chain uses.
		 * @return The unique pointer of the object.
		 */
		static std::unique_ptr<FRenderTargetAttachment> Create(FDisplayHandle displayHandle) { return std::make_unique<FSwapChainAttachment>(displayHandle); }

	public:
		FDisplayHandle mDisplayHandle = {};
	};

	/**
	 * Flint color buffer attachment object.
	 * This object contains information about the color buffer used by the render target.
	 */
	class FLINT_API FColorBufferAttachment final : public FRenderTargetAttachment
	{
	public:
		FColorBufferAttachment() : FRenderTargetAttachment(RenderTargetAttachmentType::COLOR_BUFFER) {}

		/**
		 * Create a new color buffer attachment.
		 * 
		 * @return The unique pointer of the object.
		 */
		static std::unique_ptr<FRenderTargetAttachment> Create() { return std::make_unique<FColorBufferAttachment>(); }
	};

	/**
	 * Flint depth buffer attachment object.
	 * This object contains information about the depth buffer used by the render target.
	 */
	class FLINT_API FDepthBufferAttachment final : public FRenderTargetAttachment
	{
	public:
		FDepthBufferAttachment() : FRenderTargetAttachment(RenderTargetAttachmentType::DEPTH_BUFFER) {}

		/**
		 * Create a new depth buffer attachment.
		 *
		 * @return The unique pointer of the object.
		 */
		static std::unique_ptr<FRenderTargetAttachment> Create() { return std::make_unique<FDepthBufferAttachment>(); }
	};

	/**
	 * Flint render target manager object.
	 * This object manages all the aspects of render targets.
	 */
	class FLINT_API FRenderTargetManager
	{
	public:
		FRenderTargetManager() = default;

		/**
		 * Create a new render target.
		 *
		 * @param deviceHandle: The device handle of the render target.
		 * @param pAttachments: The render target attachments.
		 * @param extent: The frame buffer extent.
		 * @param bufferCount: The number of buffers in the frame buffer. If set to 0, the default amount is set. If set to UI32_MAX, the maximum supported is set.
		 * @return The created render target handle.
		 */
		virtual FRenderTargetHandle CreateRenderTarget(const FDeviceHandle& deviceHandle, const std::vector<std::unique_ptr<FRenderTargetAttachment>>& pAttachments, const FExtent2D& extent, UI32 bufferCount = 0) = 0;

		/**
		 * Destroy a created render target.
		 *
		 * @param handle: The render target handle.
		 */
		virtual void DestroyRenderTarget(FRenderTargetHandle& handle) = 0;
	};
}