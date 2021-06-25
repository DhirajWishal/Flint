// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Components/WireFrame.hpp"

#include "VertexBuffer.hpp"
#include "IndexBuffer.hpp"

namespace Flint
{
	/**
	 * Flint wire frame storage.
	 * This object contains information about multiple wire frame object data.
	 */
	class WireFrameStorage
	{
	public:
		WireFrameStorage(VertexBuffer& vertexBuffer, IndexBuffer& indexBuffer) : mVertexBuffer(vertexBuffer), mIndexBuffer(indexBuffer) {}

	private:
		VertexBuffer& mVertexBuffer;
		IndexBuffer& mIndexBuffer;
	};
}