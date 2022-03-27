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
	template<class DeviceT, class ResourcePackageT>
	class ResourcePackager : public DeviceBoundObject<DeviceT>
	{
	public:
		/**
		 * Construct the packager.
		 *
		 * @param pPipeline The pipeline pointer.
		 */
		ResourcePackager(DeviceT* pDevice, const uint32_t setIndex)
			: DeviceBoundObject(pDevice), mSetIndex(setIndex)
		{
			if (!pPipeline)
				throw std::invalid_argument("The pipeline pointer should not be null!");
		}

		/**
		 * Create a new package.
		 * 
		 * @return The resource package.
		 */
		virtual std::shared_ptr<ResourcePackageT> CreatePackage() = 0;

	public:
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
		std::vector<std::shared_ptr<ResourcePackageT>> pPackages = {};
		std::unordered_map<uint32_t, ShaderResourceType> mResources = {};

		uint32_t mSetIndex = 0;
	};
}