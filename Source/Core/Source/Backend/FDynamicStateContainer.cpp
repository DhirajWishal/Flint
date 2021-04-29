// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core\Backend\FDynamicStateContainer.h"

namespace Flint
{
	FDynamicStateContainer::FDynamicStateContainer(const FDynamicStateContainer& other)
		: pDynamicStates(other.pDynamicStates)
	{
	}

	FDynamicStateContainer::FDynamicStateContainer(FDynamicStateContainer&& other) noexcept
		: pDynamicStates(std::move(other.pDynamicStates))
	{
	}

	FDynamicStateContainer::~FDynamicStateContainer()
	{
	}

	void FDynamicStateContainer::AddViewPort(const Vector2& extent, const Vector2& depth, const Vector2& offset)
	{
		INSERT_INTO_VECTOR(pDynamicStates, std::make_shared<ViewPort>(extent, offset, depth));
	}

	void FDynamicStateContainer::AddScissor(const Vector2& extent, const Vector2& offset)
	{
		INSERT_INTO_VECTOR(pDynamicStates, std::make_shared<Scissor>(extent, offset));
	}

	void FDynamicStateContainer::AddLineWidth(const float width)
	{
		INSERT_INTO_VECTOR(pDynamicStates, std::make_shared<LineWidth>(width));
	}

	void FDynamicStateContainer::AddDepthBias(const float biasFactor, const float clampFactor, const float slopeFactor)
	{
		INSERT_INTO_VECTOR(pDynamicStates, std::make_shared<DepthBias>(biasFactor, clampFactor, slopeFactor));
	}

	void FDynamicStateContainer::AddBlendConstants(const float(&constants)[4])
	{
		INSERT_INTO_VECTOR(pDynamicStates, std::make_shared<BlendConstants>(constants));
	}

	void FDynamicStateContainer::AddDepthBounds(const Vector2& bounds)
	{
		INSERT_INTO_VECTOR(pDynamicStates, std::make_shared<DepthBounds>(bounds));
	}

	FDynamicStateContainer& FDynamicStateContainer::operator=(const FDynamicStateContainer& other)
	{
		pDynamicStates = other.pDynamicStates;
		return *this;
	}

	FDynamicStateContainer& FDynamicStateContainer::operator=(FDynamicStateContainer&& other) noexcept
	{
		pDynamicStates = std::move(other.pDynamicStates);
		return *this;
	}
}