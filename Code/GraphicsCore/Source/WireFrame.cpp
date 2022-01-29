// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/WireFrame.hpp"

namespace Flint
{
	WireFrame::WireFrame(const std::string& name, uint64 vertexOffset, uint64 vertexCount, uint64 indexOffset, uint64 indexCount)
		: mName(name), mVertexOffset(vertexOffset), mVertexCount(vertexCount), mIndexOffset(indexOffset), mIndexCount(indexCount)
	{
	}

	WireFrame::WireFrame(const std::string& name, uint64 vertexOffset, uint64 vertexCount, uint64 indexOffset, uint64 indexCount, const Material& material)
		: mName(name), mVertexOffset(vertexOffset), mVertexCount(vertexCount), mIndexOffset(indexOffset), mIndexCount(indexCount), mMaterial(material)
	{
	}
}