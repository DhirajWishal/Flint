// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "DynamicStateContainer.hpp"

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

	void DynamicStateContainer::SetConstantData(ShaderType shaderType, void* pData, UI64 size, UI64 offset)
	{
		switch (shaderType)
		{
		case Flint::ShaderType::VERTEX:
			mConstantBlocks[0] = ConstantData(pData, size, offset);
			break;

		case Flint::ShaderType::TESSELLATION_CONTROL:
			mConstantBlocks[1] = ConstantData(pData, size, offset);
			break;

		case Flint::ShaderType::TESSELLATION_EVALUATION:
			mConstantBlocks[2] = ConstantData(pData, size, offset);
			break;

		case Flint::ShaderType::GEOMETRY:
			mConstantBlocks[3] = ConstantData(pData, size, offset);
			break;

		case Flint::ShaderType::FRAGMENT:
			mConstantBlocks[4] = ConstantData(pData, size, offset);
			break;

		case Flint::ShaderType::COMPUTE:
			mConstantBlocks[5] = ConstantData(pData, size, offset);
			break;

		case Flint::ShaderType::RAY_GEN:
			mConstantBlocks[6] = ConstantData(pData, size, offset);
			break;

		case Flint::ShaderType::ANY_HIT:
			mConstantBlocks[7] = ConstantData(pData, size, offset);
			break;

		case Flint::ShaderType::CLOSEST_HIT:
			mConstantBlocks[8] = ConstantData(pData, size, offset);
			break;

		case Flint::ShaderType::RAY_MISS:
			mConstantBlocks[9] = ConstantData(pData, size, offset);
			break;

		default:
			FLINT_THROW_INVALID_ARGUMENT("Invalid shader type passed to set constant data!");
		}
	}
}