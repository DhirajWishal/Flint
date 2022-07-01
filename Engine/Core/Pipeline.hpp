// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"
#include "PipelineCacheHandler.hpp"

namespace Flint
{
	/**
	 * Pipeline class.
	 * This is the base class for all the supported pipelines in the engine.
	 *
	 * Pipelines are used for rendering (ray tracing and raster graphics) and compute operations.
	 */
	class Pipeline : public DeviceBoundObject
	{
	public:
		/**
		 * Explicit constructor.
		 *
		 * @param pDevice The device pointer.
		 * @param pCacheHandler The pipeline cache handler used to handle the pipeline cache. Default is nullptr.
		 */
		explicit Pipeline(const std::shared_ptr<Device>& pDevice, std::unique_ptr<PipelineCacheHandler>&& pCacheHandler = nullptr)
			: DeviceBoundObject(pDevice), m_pCacheHandler(std::move(pCacheHandler)) {}

		/**
		 * Default virtual destructor.
		 */
		virtual ~Pipeline() = default;

	protected:
		std::unique_ptr<PipelineCacheHandler> m_pCacheHandler = nullptr;
	};
}