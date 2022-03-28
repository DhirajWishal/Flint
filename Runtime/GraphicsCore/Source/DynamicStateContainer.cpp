// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/DynamicStateContainer.hpp"

namespace Flint
{
	void DynamicStateContainer::SetViewPort(const FExtent2D<float>& extent, const FExtent2D<float>& depth, const FExtent2D<float>& offset)
	{
		m_ViewPort = ViewPort(extent, offset, depth);
		m_Flags = m_Flags | DynamicStateFlags::ViewPort;
	}

	void DynamicStateContainer::SetScissor(const FBox2D& extent, const FBox2D& offset)
	{
		m_Scissor = Scissor(extent, offset);
		m_Flags = m_Flags | DynamicStateFlags::Scissor;
	}

	void DynamicStateContainer::SetLineWidth(const float width)
	{
		m_LineWidth = LineWidth(width);
		m_Flags = m_Flags | DynamicStateFlags::LineWidth;
	}

	void DynamicStateContainer::SetDepthBias(const float biasFactor, const float clampFactor, const float slopeFactor)
	{
		m_DepthBias = DepthBias(biasFactor, clampFactor, slopeFactor);
		m_Flags = m_Flags | DynamicStateFlags::DepthBias;
	}

	void DynamicStateContainer::SetBlendConstants(const float(&constants)[4])
	{
		m_BlendConstants = BlendConstants(constants);
		m_Flags = m_Flags | DynamicStateFlags::BlendConstants;
	}

	void DynamicStateContainer::SetDepthBounds(const FExtent2D<float>& bounds)
	{
		m_DepthBounds = DepthBounds(bounds);
		m_Flags = m_Flags | DynamicStateFlags::DepthBounds;
	}

	void DynamicStateContainer::SetConstantData(const ShaderType shaderType, const void* pData, const uint64_t size, const uint64_t offset)
	{
		switch (shaderType)
		{
		case Flint::ShaderType::Vertex:
			m_ConstantBlocks[0] = ConstantData(pData, size, offset);
			break;

		case Flint::ShaderType::TessellationControl:
			m_ConstantBlocks[1] = ConstantData(pData, size, offset);
			break;

		case Flint::ShaderType::TessellationEvaluation:
			m_ConstantBlocks[2] = ConstantData(pData, size, offset);
			break;

		case Flint::ShaderType::Geometry:
			m_ConstantBlocks[3] = ConstantData(pData, size, offset);
			break;

		case Flint::ShaderType::Fragment:
			m_ConstantBlocks[4] = ConstantData(pData, size, offset);
			break;

		case Flint::ShaderType::Compute:
			m_ConstantBlocks[5] = ConstantData(pData, size, offset);
			break;

		case Flint::ShaderType::RayGen:
			m_ConstantBlocks[6] = ConstantData(pData, size, offset);
			break;

		case Flint::ShaderType::AnyHit:
			m_ConstantBlocks[7] = ConstantData(pData, size, offset);
			break;

		case Flint::ShaderType::ClosestHit:
			m_ConstantBlocks[8] = ConstantData(pData, size, offset);
			break;

		case Flint::ShaderType::RayMiss:
			m_ConstantBlocks[9] = ConstantData(pData, size, offset);
			break;

		default:
			throw std::invalid_argument("Invalid shader type passed to set constant data!");
		}
	}
}