// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "CommandBuffer.hpp"
#include "ComputePipeline.hpp"
#include "Core/CountingSemaphore.hpp"

#include <unordered_map>

namespace Flint
{
	class GraphicsPipeline;
	class GeometryStore;

	using DrawInstanceMap = std::unordered_map<std::shared_ptr<GeometryStore>, std::vector<std::shared_ptr<GraphicsPipeline>>>;

	/**
	 * Draw instance storage structure.
	 * This structure holds information about draw instances.
	 */
	struct DrawInstanceStorage
	{
		std::vector<DrawInstanceMap> mDrawMaps;
		std::vector<std::list<std::shared_ptr<GeometryStore>>> mDrawOrder;
	};

	struct BeginFrameInfo
	{
		std::shared_ptr<CommandBuffer> pCommandBuffer = nullptr;
		UI32 mFrameIndex = 0;

		DrawInstanceStorage const* pDrawInstanceStorage = nullptr;
	};

	/**
	 * Render target attachment structure.
	 */
	struct RenderTargetAttachment
	{
		struct DepthClearValues
		{
			DepthClearValues() = default;
			DepthClearValues(float depth, UI32 stencil) : mDepth(depth), mStencil(stencil) {}

			float mDepth = 0.0f;
			UI32 mStencil = 0;
		};

	public:
		RenderTargetAttachment() = default;
		RenderTargetAttachment(const std::shared_ptr<Image>& pImage, const FColor4D& clearColor) : pImage(pImage), mClearColor(clearColor) {}
		RenderTargetAttachment(const std::shared_ptr<Image>& pImage, const DepthClearValues& depthValue) : pImage(pImage), mDepthClearValue(depthValue) {}

		std::shared_ptr<Image> pImage = nullptr;

		FColor4D mClearColor = FColor4D(CREATE_COLOR_256(32.0f), CREATE_COLOR_256(32.0f), CREATE_COLOR_256(32.0f), 1.0f);
		DepthClearValues mDepthClearValue = {};
	};

	/**
	 * Flint render target object.
	 * This object is the base class for all the supported render targets.
	 *
	 * Render target objects are used to submit data to the device to be rendered or for compute purposes.
	 *
	 * Render targets can use secondary threads to speed up rendering. By default, the number of threads used per render target is 0, though it can be set to a specified value.
	 * Load balancing is done automatically by the render target using the following equation.
	 *		Load Per Thread = Draw Instances / Thread Count
	 * Each thread contains its own pair of GeometryStore and pipelines. So we recommend having one thread per geometry store in use IF the pipeline count or draw data count is high.
	 */
	class RenderTarget : public DeviceBoundObject
	{
	public:
		/**
		 * Default constructor.
		 *
		 * @param pDevice: The device pointer.
		 * @param extent: The render target extent.
		 * @param bufferCount: The frame buffer count.
		 */
		RenderTarget(const std::shared_ptr<Device>& pDevice, const FBox2D& extent, const UI32 bufferCount, const std::vector<RenderTargetAttachment>& imageAttachments);

		/**
		 * Begin a new frame to render.
		 *
		 * @return The frame begin info structure.
		 */
		virtual BeginFrameInfo BeginFrame() = 0;

		/**
		 * Submit the frame to the device to be rendered.
		 */
		virtual void SubmitFrame() = 0;

	public:
		/**
		 * Get the attachments of the render target.
		 *
		 * @return The attachments.
		 */
		const std::vector<RenderTargetAttachment> GetAttachments() const { return mAttachments; }

		/**
		 * Get the current frame index.
		 *
		 * @return The frame index.
		 */
		const UI32 GetFrameIndex() const { return mFrameIndex; }

		/**
		 * Get the render target extent.
		 *
		 * @return The extent.
		 */
		const FBox2D GetExtent() const { return mExtent; }

		/**
		 * Get the render target buffer count.
		 *
		 * @return The buffer count.
		 */
		const UI32 GetBufferCount() const { return mBufferCount; }

		/**
		 * Check if the render target is altered.
		 *
		 * @return Boolean value.
		 */
		const bool IsAltered() const { return bIsAltered; }

		/**
		 * Flag the render target that an alteration is made.
		 */
		void FlagAltered() { bIsAltered = true; }

		/**
		 * Increment the frame index.
		 * If the frame index is bigger than or equal to the buffer count, it is rolled back to 0.
		 */
		void IncrementFrameIndex() { mFrameIndex++; if (mFrameIndex >= mBufferCount) mFrameIndex = 0; }

	protected:
		std::vector<RenderTargetAttachment> mAttachments = {};

		FBox2D mExtent = {};
		UI32 mBufferCount = 0;
		UI32 mFrameIndex = 0;

		bool bIsAltered = false;
	};
}