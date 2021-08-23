// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

#include <memory>

namespace Flint
{
	/**
	 * Dynamic state flags.
	 * This determines which dynamic states the pipeline consists.
	 */
	enum class DynamicStateFlags : UI8 {
		UNDEFINED,
		VIEWPORT = BIT_SHIFT(0),
		SCISSOR = BIT_SHIFT(1),
		LINE_WIDTH = BIT_SHIFT(2),
		DEPTH_BIAS = BIT_SHIFT(3),
		BLEND_CONSTANTS = BIT_SHIFT(4),
		DEPTH_BOUNDS = BIT_SHIFT(5),
	};

	/**
	 * Bitwise OR operator for the dynamic state flags.
	 *
	 * @param lhs: The lhs argument.
	 * @param rhs: The rhs argument.
	 * @return The result.
	 */
	constexpr DynamicStateFlags operator|(const DynamicStateFlags& lhs, const DynamicStateFlags& rhs)
	{
		return static_cast<DynamicStateFlags>(static_cast<UI8>(lhs) | static_cast<UI8>(rhs));
	}

	/**
	 * Bitwise AND operator for the dynamic state flags.
	 *
	 * @param lhs: The lhs argument.
	 * @param rhs: The rhs argument.
	 * @return The result.
	 */
	constexpr bool operator&(const DynamicStateFlags& lhs, const DynamicStateFlags& rhs)
	{
		return static_cast<UI8>(lhs) & static_cast<UI8>(rhs);
	}

	/**
	 * Flint dynamic state container.
	 * This object contains the dynamic states which are to be passed to the pipeline at draw time.
	 */
	struct DynamicStateContainer
	{
		/**
		 * Dynamic sate object.
		 * This is the base class for all the dynamic states supported.
		 */
		struct DynamicStateObject
		{
			DynamicStateObject(DynamicStateFlags flag) : mFlag(flag) {}
			virtual ~DynamicStateObject() {}

			/**
			 * Get the state flags of the object.
			 *
			 * @return The dynamic state flag.
			 */
			DynamicStateFlags GetFlag() const { return mFlag; }

			DynamicStateFlags mFlag = DynamicStateFlags::UNDEFINED;
		};

		/**
		 * Viewport structure.
		 * Viewport is the area that is being rendered to the screen. Think of it as a window and the content within the boundary will be displayed.
		 */
		struct ViewPort final : public DynamicStateObject
		{
			ViewPort() : DynamicStateObject(DynamicStateFlags::VIEWPORT) {}
			ViewPort(const FExtent2D<float>& extent, const FExtent2D<float>& offset, const FExtent2D<float>& depth)
				: DynamicStateObject(DynamicStateFlags::VIEWPORT), mExtent(extent), mOffset(offset), mDepth(depth) {}

			FExtent2D<float> mExtent = {};
			FExtent2D<float> mOffset = {};
			FExtent2D<float> mDepth = {};	// { Min, Max }
		};

		/**
		 * Scissor structure.
		 */
		struct Scissor final : public DynamicStateObject
		{
			Scissor() : DynamicStateObject(DynamicStateFlags::SCISSOR) {}
			Scissor(const FBox2D& extent, const FBox2D& offset) : DynamicStateObject(DynamicStateFlags::SCISSOR), mExtent(extent), mOffset(offset) {}

			FBox2D mExtent = {};
			FBox2D mOffset = {};
		};

		/**
		 * Line width structure.
		 * This defines the rendering line width.
		 */
		struct LineWidth final : public DynamicStateObject
		{
			LineWidth() : DynamicStateObject(DynamicStateFlags::LINE_WIDTH) {}
			LineWidth(const float width) : DynamicStateObject(DynamicStateFlags::LINE_WIDTH), mLineWidth(width) {}

			float mLineWidth = 1.0f;
		};

		/**
		 * Depth bias structure.
		 * Set the depth bias.
		 */
		struct DepthBias final : public DynamicStateObject
		{
			DepthBias() : DynamicStateObject(DynamicStateFlags::DEPTH_BIAS) {}
			DepthBias(const float biasFactor, const float clampFactor, const float slopeFactor)
				: DynamicStateObject(DynamicStateFlags::DEPTH_BIAS), mDepthBiasFactor(biasFactor),
				mDepthClampFactor(clampFactor), mDepthSlopeFactor(slopeFactor) {}

			float mDepthBiasFactor = 0.0f;
			float mDepthClampFactor = 0.0f;
			float mDepthSlopeFactor = 0.0f;
		};

		/**
		 * Blend constants structure.
		 * Set the pipeline blend constants.
		 */
		struct BlendConstants final : public DynamicStateObject
		{
			BlendConstants() : DynamicStateObject(DynamicStateFlags::BLEND_CONSTANTS) {}
			BlendConstants(const float(&constants)[4]) : DynamicStateObject(DynamicStateFlags::BLEND_CONSTANTS) { std::copy(constants, constants + 4, mConstants); }

			float mConstants[4] = {};
		};

		/**
		 * Depth bounds struct.
		 * Set pipeline depth bounds.
		 */
		struct DepthBounds final : public DynamicStateObject
		{
			DepthBounds() : DynamicStateObject(DynamicStateFlags::DEPTH_BOUNDS) {}
			DepthBounds(const FExtent2D<float>& bounds) : DynamicStateObject(DynamicStateFlags::DEPTH_BOUNDS), mBounds(bounds) {}

			FExtent2D<float> mBounds = {};	// { Min, Max }
		};

		/**
		 * Constant data struct.
		 * This struct is used to pass data to constant blocks.
		 */
		struct ConstantData
		{
			ConstantData() = default;
			ConstantData(const void* pData, UI64 size, UI64 offset) : pData(pData), mSize(size), mOffset(offset) {}

			/**
			 * Check if the constant block is null.
			 *
			 * @return The boolean value.
			 */
			constexpr bool IsNull() const { return pData == nullptr; }

			const void* pData = nullptr;
			UI64 mSize = 0;
			UI64 mOffset = 0;
		};

	public:
		DynamicStateContainer() = default;

		/**
		 * Set a viewport to the container.
		 *
		 * @param extent: The extent of the viewport.
		 * @param depth: The viewport depth.
		 * @pram offset: The viewport offset.
		 */
		void SetViewPort(const FExtent2D<float>& extent, const FExtent2D<float>& depth, const FExtent2D<float>& offset);

		/**
		 * Set a scissor to the container.
		 *
		 * @param extent: The scissor extent.
		 * @param offset: The scissor offset.
		 */
		void SetScissor(const FBox2D& extent, const FBox2D& offset);

		/**
		 * Set a line width to the container.
		 *
		 * @param width: The line width in float.
		 */
		void SetLineWidth(const float width);

		/**
		 * Set depth bias to the container.
		 *
		 * @param biasFactor: The depth bias factor.
		 * @param clampFactor: The depth bias clamp factor.
		 * @param slopeFacror: The depth bias slope factor.
		 */
		void SetDepthBias(const float biasFactor, const float clampFactor, const float slopeFactor);

		/**
		 * Set blend constants to the container.
		 *
		 * @param constants: The static float[4] constants.
		 */
		void SetBlendConstants(const float(&constants)[4]);

		/**
		 * Set depth bounds to the container.
		 *
		 * @param bounds: The depth bounds.
		 */
		void SetDepthBounds(const FExtent2D<float>& bounds);

		/**
		 * Set constant data which is to be submitted to shaders.
		 *
		 * @param shaderType: The shader type to which the data is submitted to.
		 * @param pData: The data pointer.
		 * @param size: The size to be passed.
		 * @param offset: The offset of the data. Default is 0.
		 */
		void SetConstantData(ShaderType shaderType, void* pData, UI64 size, UI64 offset = 0);

	public:
		ViewPort mViewPort = {};
		Scissor mScissor = {};
		LineWidth mLineWidth = {};
		DepthBias mDepthBias = {};
		BlendConstants mBlendConstants = {};
		DepthBounds mDepthBounds = {};

		DynamicStateFlags mFlags = DynamicStateFlags::UNDEFINED;

		ConstantData mConstantBlocks[10] = {};
	};
}