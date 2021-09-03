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
		ResourcePackager(const std::shared_ptr<GraphicsPipeline>& pPipeline);

		/**
		 * Construct the packager.
		 *
		 * @param pPipeline: The compute pipeline pointer.
		 */
		ResourcePackager(const std::shared_ptr<ComputePipeline>& pPipeline);

	public:
		/**
		 * Get the set indexes of the packager.
		 *
		 * @return The set index vector.
		 */
		const std::vector<UI32> GetSetIndexes() const;

	protected:
		std::shared_ptr<Pipeline> pPipeline = nullptr;

		std::unordered_map<UI32, std::vector<std::shared_ptr<ResourcePackage>>> pPackageMap;
	};
}