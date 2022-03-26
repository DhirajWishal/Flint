// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/DynamicStateContainer.hpp"

namespace Flint
{
	void DynamicStateContainer::SetViewPort(const FExtent2D<float>& extent, const FExtent2D<float>& depth, const FExtent2D<float>& offset)
	{
		mViewPort = ViewPort(extent, offset, depth);
		mFlags = mFlags | DynamicStateFlags::ViewPort;
	}

	void DynamicStateContainer::SetScissor(const FBox2D& extent, const FBox2D& offset)
	{
		mScissor = Scissor(extent, offset);
		mFlags = mFlags | DynamicStateFlags::Scissor;
	}

	void DynamicStateContainer::SetLineWidth(const float width)
	{
		mLineWidth = LineWidth(width);
		mFlags = mFlags | DynamicStateFlags::LineWidth;
	}

	void DynamicStateContainer::SetDepthBias(const float biasFactor, const float clampFactor, const float slopeFactor)
	{
		mDepthBias = DepthBias(biasFactor, clampFactor, slopeFactor);
		mFlags = mFlags | DynamicStateFlags::DepthBias;
	}

	void DynamicStateContainer::SetBlendConstants(const float(&constants)[4])
	{
		mBlendConstants = BlendConstants(constants);
		mFlags = mFlags | DynamicStateFlags::BlendConstants;
	}

	void DynamicStateContainer::SetDepthBounds(const FExtent2D<float>& bounds)
	{
		mDepthBounds = DepthBounds(bounds);
		mFlags = mFlags | DynamicStateFlags::DepthBounds;
	}

	void DynamicStateContainer::SetConstantData(const ShaderType shaderType, const void* pData, const uint64 size, const uint64 offset)
	{
		switch (shaderType)
		{
		case Flint::ShaderType::Vertex:
			mConstantBlocks[0] = ConstantData(pData, size, offset);
			break;

		case Flint::ShaderType::TessellationControl:
			mConstantBlocks[1] = ConstantData(pData, size, offset);
			break;

		case Flint::ShaderType::TessellationEvaluation:
			mConstantBlocks[2] = ConstantData(pData, size, offset);
			break;

		case Flint::ShaderType::Geometry:
			mConstantBlocks[3] = ConstantData(pData, size, offset);
			break;

		case Flint::ShaderType::Fragment:
			mConstantBlocks[4] = ConstantData(pData, size, offset);
			break;

		case Flint::ShaderType::Compute:
			mConstantBlocks[5] = ConstantData(pData, size, offset);
			break;

		case Flint::ShaderType::RayGen:
			mConstantBlocks[6] = ConstantData(pData, size, offset);
			break;

		case Flint::ShaderType::AnyHit:
			mConstantBlocks[7] = ConstantData(pData, size, offset);
			break;

		case Flint::ShaderType::ClosestHit:
			mConstantBlocks[8] = ConstantData(pData, size, offset);
			break;

		case Flint::ShaderType::RayMiss:
			mConstantBlocks[9] = ConstantData(pData, size, offset);
			break;

		default:
			throw std::invalid_argument("Invalid shader type passed to set constant data!");
		}
	}
}