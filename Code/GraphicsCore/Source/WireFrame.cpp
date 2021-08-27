// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/WireFrame.hpp"

namespace Flint
{
	WireFrame::WireFrame(const std::string& name, UI64 vertexOffset, UI64 vertexCount, UI64 indexOffset, UI64 indexCount)
		: mName(name), mVertexOffset(vertexOffset), mVertexCount(vertexCount), mIndexOffset(indexOffset), mIndexCount(indexCount)
	{
	}
}