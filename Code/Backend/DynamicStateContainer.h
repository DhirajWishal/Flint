// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/DataType.hpp"

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
	struct DynamicStateContainer {
		/**
		 * Dynamic sate object.
		 * This is the base class for all the dynamic states supported.
		 */
		struct DynamicStateObject {
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
		struct ViewPort : public DynamicStateObject {
			ViewPort() : DynamicStateObject(DynamicStateFlags::VIEWPORT) {}
			ViewPort(const FExtent2D& extent, const FExtent2D& offset, const FExtent2D& depth)
				: DynamicStateObject(DynamicStateFlags::VIEWPORT), mExtent(extent), mOffset(offset), mDepth(depth) {}

			FExtent2D mExtent = {};
			FExtent2D mOffset = {};
			FExtent2D mDepth = {};	// { Min, Max }
		};

		/**
		 * Scissor structure.
		 */
		struct Scissor : public DynamicStateObject {
			Scissor() : DynamicStateObject(DynamicStateFlags::SCISSOR) {}
			Scissor(const FExtent2D& extent, const FExtent2D& offset) : DynamicStateObject(DynamicStateFlags::SCISSOR), mExtent(extent), mOffset(offset) {}

			FExtent2D mExtent = {};
			FExtent2D mOffset = {};
		};

		/**
		 * Line width structure.
		 * This defines the rendering line width.
		 */
		struct LineWidth : public DynamicStateObject {
			LineWidth() : DynamicStateObject(DynamicStateFlags::LINE_WIDTH) {}
			LineWidth(const float width) : DynamicStateObject(DynamicStateFlags::LINE_WIDTH), mLineWidth(width) {}

			float mLineWidth = 1.0f;
		};

		/**
		 * Depth bias structure.
		 * Set the depth bias.
		 */
		struct DepthBias : public DynamicStateObject {
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
		struct BlendConstants : public DynamicStateObject {
			BlendConstants() : DynamicStateObject(DynamicStateFlags::BLEND_CONSTANTS) {}
			BlendConstants(const float(&constants)[4]) : DynamicStateObject(DynamicStateFlags::BLEND_CONSTANTS) { std::copy(constants, constants + 4, mConstants); }

			float mConstants[4] = {};
		};

		/**
		 * Depth bounds struct.
		 * Set pipeline depth bounds.
		 */
		struct DepthBounds : public DynamicStateObject {
			DepthBounds() : DynamicStateObject(DynamicStateFlags::DEPTH_BOUNDS) {}
			DepthBounds(const FExtent2D& bounds) : DynamicStateObject(DynamicStateFlags::DEPTH_BOUNDS), mBounds(bounds) {}

			FExtent2D mBounds = {};	// { Min, Max }
		};

	public:
		DynamicStateContainer() = default;

		/**
		 * Add a viewport to the container.
		 *
		 * @param extent: The extent of the viewport.
		 * @param depth: The viewport depth.
		 * @pram offset: The viewport offset.
		 */
		void AddViewPort(const FExtent2D& extent, const FExtent2D& depth, const FExtent2D& offset);

		/**
		 * Add a scissor to the container.
		 *
		 * @param extent: The scissor extent.
		 * @param offset: The scissor offset.
		 */
		void AddScissor(const FExtent2D& extent, const FExtent2D& offset);

		/**
		 * Add a line width to the container.
		 *
		 * @param width: The line width in float.
		 */
		void AddLineWidth(const float width);

		/**
		 * Add depth bias to the container.
		 *
		 * @param biasFactor: The depth bias factor.
		 * @param clampFactor: The depth bias clamp factor.
		 * @param slopeFacror: The depth bias slope factor.
		 */
		void AddDepthBias(const float biasFactor, const float clampFactor, const float slopeFactor);

		/**
		 * Add blend constants to the container.
		 *
		 * @param constants: The static float[4] constants.
		 */
		void AddBlendConstants(const float(&constants)[4]);

		/**
		 * Add depth bounds to the container.
		 *
		 * @param bounds: The depth bounds.
		 */
		void AddDepthBounds(const FExtent2D& bounds);

		std::vector<std::shared_ptr<DynamicStateObject>> pDynamicStates;
	};
}