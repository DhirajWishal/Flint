// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/RasterizingPipeline.hpp"
#include "Core/Errors/InvalidArgumentError.hpp"

namespace Flint
{
	namespace Backend
	{
		RasterizingPipeline::RasterizingPipeline(const std::shared_ptr<Device>& pDevice, const std::shared_ptr<Rasterizer>& pRasterizer, const std::shared_ptr<RasterizingProgram>& pProgram, const RasterizingPipelineSpecification& specification, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler /*= nullptr*/)
			: Pipeline(pDevice, std::move(pCacheHandler)), m_pRasterizer(pRasterizer), m_pProgram(pProgram), m_Specification(specification)
		{
			if (!pProgram)
				throw InvalidArgumentError("Program pointer should not be null!");
		}
	}
}