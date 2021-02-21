// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Objects/WireFrame.h"
#include "Core/ErrorHandler/Logger.h"

namespace Flint
{
	std::vector<VertexAttribute> CreateDefaultAttributes()
	{
		std::vector<VertexAttribute> attributes(1);

		attributes[0].mType = VertexAttributeType::POSITION;
		attributes[0].mSize = sizeof(float) * 4;

		return attributes;
	}
	
	WireFrame::WireFrame(const WireFrame& other)
		: pVertexBuffer(other.pVertexBuffer), pIndexBuffer(other.pIndexBuffer), mDrawData(other.mDrawData), mAttributes(other.mAttributes)
	{
	}

	WireFrame::WireFrame(WireFrame&& other) noexcept
		: pVertexBuffer(other.pVertexBuffer), pIndexBuffer(other.pIndexBuffer), mDrawData(std::move(other.mDrawData)), mAttributes(std::move(other.mAttributes))
	{
		other.pVertexBuffer = nullptr;
		other.pIndexBuffer = nullptr;
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
		mDrawData = other.mDrawData;

		return *this;
	}
	
	WireFrame& WireFrame::operator=(WireFrame&& other) noexcept
	{
		pVertexBuffer = other.pVertexBuffer;
		pIndexBuffer = other.pIndexBuffer;
		mAttributes = std::move(other.mAttributes);
		mDrawData = std::move(other.mDrawData);

		other.pVertexBuffer = nullptr;
		other.pIndexBuffer = nullptr;

		return *this;
	}
}