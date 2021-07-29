// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DynamicStateContainer.h"

namespace Flint
{
	void DynamicStateContainer::AddViewPort(const FExtent2D& extent, const FExtent2D& depth, const FExtent2D& offset)
	{
		INSERT_INTO_VECTOR(pDynamicStates, std::make_shared<ViewPort>(extent, offset, depth));
	}

	void DynamicStateContainer::AddScissor(const FExtent2D& extent, const FExtent2D& offset)
	{
		INSERT_INTO_VECTOR(pDynamicStates, std::make_shared<Scissor>(extent, offset));
	}

	void DynamicStateContainer::AddLineWidth(const float width)
	{
		INSERT_INTO_VECTOR(pDynamicStates, std::make_shared<LineWidth>(width));
	}

	void DynamicStateContainer::AddDepthBias(const float biasFactor, const float clampFactor, const float slopeFactor)
	{
		INSERT_INTO_VECTOR(pDynamicStates, std::make_shared<DepthBias>(biasFactor, clampFactor, slopeFactor));
	}

	void DynamicStateContainer::AddBlendConstants(const float(&constants)[4])
	{
		INSERT_INTO_VECTOR(pDynamicStates, std::make_shared<BlendConstants>(constants));
	}

	void DynamicStateContainer::AddDepthBounds(const FExtent2D& bounds)
	{
		INSERT_INTO_VECTOR(pDynamicStates, std::make_shared<DepthBounds>(bounds));
	}
}