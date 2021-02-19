// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint/Components/WireFrame.h"
#include "Core/ErrorHandler/Logger.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace Flint
{
	std::vector<VertexAttribute> CreateDefaultAttributes()
	{
		std::vector<VertexAttribute> attributes(1);

		attributes[0].mType = VertexAttributeType::POSITION;
		attributes[0].mSize = sizeof(float) * 4;

		return attributes;
	}
	
	void WireFrame::Clear()
	{
		if (pVertexBuffer) pVertexBuffer->Terminate(), delete pVertexBuffer;
		if (pIndexBuffer) pIndexBuffer->Terminate(), delete pIndexBuffer;

		mDrawData.clear();
	}
	
	WireFrame& WireFrame::operator=(const WireFrame& other)
	{
		pVertexBuffer = other.pVertexBuffer;
		pIndexBuffer = other.pIndexBuffer;
		mAttributes = other.mAttributes;

		return *this;
	}
}