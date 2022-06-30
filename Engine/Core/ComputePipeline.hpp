// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Pipeline.hpp"

namespace Flint
{
	/**
	 * Compute pipeline class.
	 * This class is used to perform compute operations.
	 */
	class ComputePipeline : public Pipeline
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param pDevice The device pointer.
		 * @param pCacheHandler The pipeline cache handler used to handle the pipeline cache. Default is nullptr.
		 */
		explicit ComputePipeline(const std::shared_ptr<Device>& pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler = nullptr) : Pipeline(pDevice, std::move(pCacheHandler)) {}

		/**
		 * Default virtual destructor.
		 */
		virtual ~ComputePipeline() = default;
	};
}