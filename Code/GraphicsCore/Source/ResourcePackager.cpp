// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/ResourcePackager.hpp"

namespace Flint
{
	ResourcePackager::ResourcePackager(const std::shared_ptr<GraphicsPipeline>& pPipeline)
		: pPipeline(pPipeline)
	{
		if (!pPipeline)
			throw std::invalid_argument("The pipeline pointer should not be null!");
	}

	ResourcePackager::ResourcePackager(const std::shared_ptr<ComputePipeline>& pPipeline)
		: pPipeline(pPipeline)
	{
		if (!pPipeline)
			throw std::invalid_argument("The pipeline pointer should not be null!");
	}

	const std::vector<UI32> ResourcePackager::GetSetIndexes() const
	{
		std::vector<UI32> sets = {};
		for (const auto entries : pPackageMap)
			sets.push_back(entries.first);

		return sets;
	}
}