// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Pipeline.hpp"

namespace Flint
{
	/**
	 * Ray tracing pipeline class.
	 * This pipeline is used to perform ray tracing.
	 */
	class RayTracingPipeline : public Pipeline
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param pDevice The device pointer.
		 * @param pCacheHandler The pipeline cache handler used to handle the pipeline cache. Default is nullptr.
		 */
		explicit RayTracingPipeline(const std::shared_ptr<Device>& pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler = nullptr) : Pipeline(pDevice, std::move(pCacheHandler)) {}

		/**
		 * Default virtual destructor.
		 */
		virtual ~RayTracingPipeline() = default;
	};
}