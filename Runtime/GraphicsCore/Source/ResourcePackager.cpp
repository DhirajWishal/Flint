// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/ResourcePackager.hpp"

namespace Flint
{
	ResourcePackager::ResourcePackager(const uint32_t setIndex, const GraphicsPipeline* pPipeline)
		: mSetIndex(setIndex), pPipeline(pPipeline)
	{
		if (!pPipeline)
			throw std::invalid_argument("The pipeline pointer should not be null!");
	}

	ResourcePackager::ResourcePackager(const uint32_t setIndex, const ComputePipeline* pPipeline)
		: mSetIndex(setIndex), pPipeline(pPipeline)
	{
		if (!pPipeline)
			throw std::invalid_argument("The pipeline pointer should not be null!");
	}
}