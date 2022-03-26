// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/ResourcePackager.hpp"

namespace Flint
{
	ResourcePackager::ResourcePackager(const uint32 setIndex, const std::shared_ptr<GraphicsPipeline>& pPipeline)
		: mSetIndex(setIndex), pPipeline(pPipeline)
	{
		if (!pPipeline)
			throw std::invalid_argument("The pipeline pointer should not be null!");
	}

	ResourcePackager::ResourcePackager(const uint32 setIndex, const std::shared_ptr<ComputePipeline>& pPipeline)
		: mSetIndex(setIndex), pPipeline(pPipeline)
	{
		if (!pPipeline)
			throw std::invalid_argument("The pipeline pointer should not be null!");
	}
}