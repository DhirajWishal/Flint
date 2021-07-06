// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Components/WireFrame.hpp"
#include "StaggingBuffer.hpp"

namespace Flint
{
	/**
	 * Wire frame payload structure.
	 * This contains the raw data which are to be sent to the
	 */
	struct WireFramePayload
	{
		std::unique_ptr<StaggingBuffer> pVertexData;
		std::unique_ptr<StaggingBuffer> pIndexData;

		UI64 mVertexSize = 0, mVertexCount = 0;
		UI64 mIndexSize = 0, mIndexCount = 0;
	};

	/**
	 * Flint wire frame loader.
	 * This class is used to import wire frame cache files, or asset files.
	 */
	class WireFrameLoader
	{
	public:
		WireFrameLoader() {}
	};
}