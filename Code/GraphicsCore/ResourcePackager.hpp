// Copyright 2021 Dhiraj Wishal
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
		 * @param pPipeline: The graphics pipeline pointer.
		 */
		ResourcePackager(const UI32 setIndex, const std::shared_ptr<GraphicsPipeline>& pPipeline);

		/**
		 * Construct the packager.
		 *
		 * @param pPipeline: The compute pipeline pointer.
		 */
		ResourcePackager(const UI32 setIndex, const std::shared_ptr<ComputePipeline>& pPipeline);

		/**
		 * Create a new package.
		 * 
		 * @return The resource package.
		 */
		virtual std::shared_ptr<ResourcePackage> CreatePackage() = 0;

	public:
		/**
		 * Get the pipeline pointer of the packager.
		 * 
		 * @return The pipeline pointer.
		 */
		const std::shared_ptr<Pipeline> GetPipeline() const { return pPipeline; }

		/**
		 * Get the set index of the packager.
		 *
		 * @return The set index.
		 */
		const UI32 GetSetIndex() const { return mSetIndex; }

		/**
		 * Get the resource map of the packager.
		 * 
		 * @return The resource map.
		 */
		const std::unordered_map<UI32, ShaderResourceType> GetResources() const { return mResources; }

	protected:
		std::shared_ptr<Pipeline> pPipeline = nullptr;
		std::vector<std::shared_ptr<ResourcePackage>> pPackages = {};
		std::unordered_map<UI32, ShaderResourceType> mResources = {};

		UI32 mSetIndex = 0;
	};
}