// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Backend/GraphicsPipeline.h"

namespace Flint
{
	namespace Backend
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
			for (auto ptr : pDynamicStates)
				delete ptr;
		}

		void DynamicStateContainer::AddViewPort(const Vector2& extent, const Vector2& depth, const Vector2& offset)
		{
			INSERT_INTO_VECTOR(pDynamicStates, new ViewPort(extent, depth, offset));
		}

		void DynamicStateContainer::AddScissor(const Vector2& extent)
		{
			INSERT_INTO_VECTOR(pDynamicStates, new Scissor(extent));
		}

		void DynamicStateContainer::AddLineWidth(const float width)
		{
			INSERT_INTO_VECTOR(pDynamicStates, new LineWidth(width));
		}

		void DynamicStateContainer::AddDepthBias(const float biasFactor, const float clampFactor, const float slopeFactor)
		{
			INSERT_INTO_VECTOR(pDynamicStates, new DepthBias(biasFactor, clampFactor, slopeFactor));
		}

		void DynamicStateContainer::AddBlendConstants(const float(&constants)[4])
		{
			INSERT_INTO_VECTOR(pDynamicStates, new BlendConstants(constants));
		}

		void DynamicStateContainer::AddDepthBounds(const Vector2& bounds)
		{
			INSERT_INTO_VECTOR(pDynamicStates, new DepthBounds(bounds));
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

	namespace GraphicsPipelinePresets
	{
		Backend::GraphicsPipelineSpecification CreateDefaultSpec()
		{
			Backend::GraphicsPipelineSpecification spec = {};
			spec.bEnableSampleShading = false;
			spec.bEnableAlphaCoverage = false;
			spec.bEnableAlphaToOne = false;
			spec.bEnableColorBlendLogic = false;
			spec.bEnableDepthTest = true;
			spec.bEnableDepthWrite = true;
			spec.mRasterizerLineWidth = 1.0f;
			spec.mMinSampleShading = 0.0f;
			spec.mColorBlendConstants[0] = 0.0f;
			spec.mColorBlendConstants[1] = 0.0f;
			spec.mColorBlendConstants[2] = 0.0f;
			spec.mColorBlendConstants[3] = 0.0f;

			return spec;
		}
	}
}