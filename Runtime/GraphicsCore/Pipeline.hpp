// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	class RenderTarget;
	class ResourcePackager;
	class ResourcePackage;

	/**
	 * Flint pipeline object.
	 * This object is used to store information about a rendering or compute pipeline.
	 */
	class Pipeline : public DeviceBoundObject
	{
		friend RenderTarget;

	public:
		/**
		 * Default constructor.
		 *
		 * @param pDevice The device pointer.
		 * @param pipelineName The name of the pipeline. This name is given to the pipeline cache object created upon destruction. Make sure that this name is unique.
		 *	If you wish to not save cache externally, keep this field empty ("").
		 */
		Pipeline(Device* pDevice, const std::string& pipelineName) : DeviceBoundObject(pDevice), mPipelineName(pipelineName) {}

		/**
		 * Reload the shaders.
		 * NOTE: This will only reload the shaders and will perform reflection on the shader code. If any input, output or uniform was changed, make sure to reflect them in
		 * input data.
		 */
		virtual void ReloadShaders() = 0;

	protected:
		/**
		 * Create the resource packagers.
		 */
		virtual void CreateResourcePackagers() = 0;

		/**
		 * Write the pipeline cache data to an external file.
		 * If the file does not exist, this creates a new file. The file name is "<pipeline name>.fpc". The FPC extension is Flint Pipeline Cache.
		 *
		 * @param size The size of the data block.
		 * @param pData The data to be written.
		 */
		void WriteDataToCacheFile(const uint64_t size, unsigned char* pData) const;

		/**
		 * Read the pipeline cache data from an external file.
		 *
		 * @return The pair of size and data.
		 */
		std::pair<uint64_t, unsigned char*> ReadDataFromCacheFile() const;

	public:
		/**
		 * Create a new resource package.
		 * 
		 * @param index The set index.
		 * @return The package.
		 */
		std::shared_ptr<ResourcePackage> CreateResourcePackage(const uint64_t index);

	protected:
		std::string mPipelineName = "";
		std::vector<std::shared_ptr<ResourcePackager>> pResourcePackagers = {};

		bool bShouldPrepareResources = true;
	};
}