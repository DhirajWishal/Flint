// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/WireFrame.hpp"

namespace Flint
{
	WireFrame::WireFrame(const std::string& name, uint64_t vertexOffset, uint64_t vertexCount, uint64_t indexOffset, uint64_t indexCount)
		: m_Name(name), m_VertexOffset(vertexOffset), m_VertexCount(vertexCount), m_IndexOffset(indexOffset), m_IndexCount(indexCount)
	{
	}

	WireFrame::WireFrame(const std::string& name, uint64_t vertexOffset, uint64_t vertexCount, uint64_t indexOffset, uint64_t indexCount, const Material& material)
		: m_Name(name), m_VertexOffset(vertexOffset), m_VertexCount(vertexCount), m_IndexOffset(indexOffset), m_IndexCount(indexCount), m_Material(material)
	{
	}
}