// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ResourcePackage.hpp"
#include "GraphicsPipeline.hpp"
#include "ComputePipeline.hpp"

namespace Flint
{
	/**
	 * Flint resource packager.
	 * Resource packager objects are used to create resource packages and also manage them.
	 */
	class ResourcePackager : public FObject
	{
	public:
		/**
		 * Construct the packager.
		 *
		 * @param pPipeline The graphics pipeline pointer.
		 */
		ResourcePackager(const uint32_t setIndex, const GraphicsPipeline* pPipeline);

		/**
		 * Construct the packager.
		 *
		 * @param pPipeline The compute pipeline pointer.
		 */
		ResourcePackager(const uint32_t setIndex, const ComputePipeline* pPipeline);

		/**
		 * Create a new package.
		 * 
		 * @return The resource package.
		 */
		virtual std::shared_ptr<ResourcePackage> CreatePackage() = 0;

		/**
		 * Terminate the resource packager.
		 */
		virtual void Terminate() = 0;

	public:
		/**
		 * Get the pipeline pointer of the packager.
		 * 
		 * @return The pipeline pointer.
		 */
		const Pipeline* GetPipeline() const { return pPipeline; }

		/**
		 * Get the set index of the packager.
		 *
		 * @return The set index.
		 */
		const uint32_t GetSetIndex() const { return mSetIndex; }

		/**
		 * Get the resource map of the packager.
		 * 
		 * @return The resource map.
		 */
		const std::unordered_map<uint32_t, ShaderResourceType> GetResources() const { return mResources; }

	protected:
		const Pipeline* pPipeline = nullptr;
		std::vector<std::shared_ptr<ResourcePackage>> pPackages = {};
		std::unordered_map<uint32_t, ShaderResourceType> mResources = {};

		uint32_t mSetIndex = 0;
	};
}