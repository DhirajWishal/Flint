// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DynamicStateContainer.h"

namespace Flint
{
	void DynamicStateContainer::SetViewPort(const FExtent2D<float>& extent, const FExtent2D<float>& depth, const FExtent2D<float>& offset)
	{
		mViewPort = ViewPort(extent, offset, depth);
		mFlags = mFlags | DynamicStateFlags::VIEWPORT;
	}

	void DynamicStateContainer::SetScissor(const FBox2D& extent, const FBox2D& offset)
	{
		mScissor = Scissor(extent, offset);
		mFlags = mFlags | DynamicStateFlags::SCISSOR;
	}

	void DynamicStateContainer::SetLineWidth(const float width)
	{
		mLineWidth = LineWidth(width);
		mFlags = mFlags | DynamicStateFlags::LINE_WIDTH;
	}

	void DynamicStateContainer::SetDepthBias(const float biasFactor, const float clampFactor, const float slopeFactor)
	{
		mDepthBias = DepthBias(biasFactor, clampFactor, slopeFactor);
		mFlags = mFlags | DynamicStateFlags::DEPTH_BIAS;
	}

	void DynamicStateContainer::SetBlendConstants(const float(&constants)[4])
	{
		mBlendConstants = BlendConstants(constants);
		mFlags = mFlags | DynamicStateFlags::BLEND_CONSTANTS;
	}

	void DynamicStateContainer::SetDepthBounds(const FExtent2D<float>& bounds)
	{
		mDepthBounds = DepthBounds(bounds);
		mFlags = mFlags | DynamicStateFlags::DEPTH_BOUNDS;
	}
}