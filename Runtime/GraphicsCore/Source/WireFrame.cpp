// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/WireFrame.hpp"

namespace Flint
{
	WireFrame::WireFrame(const std::string& name, uint64_t vertexOffset, uint64_t vertexCount, uint64_t indexOffset, uint64_t indexCount)
		: mName(name), mVertexOffset(vertexOffset), mVertexCount(vertexCount), mIndexOffset(indexOffset), mIndexCount(indexCount)
	{
	}

	WireFrame::WireFrame(const std::string& name, uint64_t vertexOffset, uint64_t vertexCount, uint64_t indexOffset, uint64_t indexCount, const Material& material)
		: mName(name), mVertexOffset(vertexOffset), mVertexCount(vertexCount), mIndexOffset(indexOffset), mIndexCount(indexCount), mMaterial(material)
	{
	}
}