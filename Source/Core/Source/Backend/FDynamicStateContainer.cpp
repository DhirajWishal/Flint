// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core\Backend\FDynamicStateContainer.h"

namespace Flint
{
	DynamicStateContainer::DynamicStateContainer(const DynamicStateContainer& other)
		: pDynamicStates(other.pDynamicStates)
	{
	}

	DynamicStateContainer::DynamicStateContainer(DynamicStateContainer&& other) noexcept
		: pDynamicStates(std::move(other.pDynamicStates))
	{
	}

	DynamicStateContainer::~DynamicStateContainer()
	{
	}

	void DynamicStateContainer::AddViewPort(const Vector2& extent, const Vector2& depth, const Vector2& offset)
	{
		INSERT_INTO_VECTOR(pDynamicStates, std::make_shared<ViewPort>(extent, offset, depth));
	}

	void DynamicStateContainer::AddScissor(const Vector2& extent, const Vector2& offset)
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

	void DynamicStateContainer::AddDepthBounds(const Vector2& bounds)
	{
		INSERT_INTO_VECTOR(pDynamicStates, std::make_shared<DepthBounds>(bounds));
	}

	DynamicStateContainer& DynamicStateContainer::operator=(const DynamicStateContainer& other)
	{
		pDynamicStates = other.pDynamicStates;
		return *this;
	}

	DynamicStateContainer& DynamicStateContainer::operator=(DynamicStateContainer&& other) noexcept
	{
		pDynamicStates = std::move(other.pDynamicStates);
		return *this;
	}
}