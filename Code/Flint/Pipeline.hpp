// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"
#include "ResourceMap.hpp"

namespace Flint
{
	class RenderTarget;

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
		 * @param pDevice: The device pointer.
		 * @param pipelineName: The name of the pipeline. This name is given to the pipeline cache object created upon destruction. Make sure that this name is unique.
		 *	If you wish to not save cache externally, keep this field empty ("").
		 */
		Pipeline(const std::shared_ptr<Device>& pDevice, const std::string& pipelineName) : DeviceBoundObject(pDevice), mPipelineName(pipelineName) {}

		/**
		 * Prepare resources to draw.
		 */
		virtual void PrepareResources() = 0;

		/**
		 * Reload the shaders.
		 * NOTE: This will only reload the shaders and will perform reflection on the shader code. If any input, output or uniform was changed, make sure to reflect them in
		 * input data.
		 */
		virtual void ReloadShaders() = 0;

		/**
		 * Create a new resource map using the provided shaders.
		 *
		 * @return The resource map pointer.
		 */
		virtual std::shared_ptr<ResourceMap> CreateResourceMap() const = 0;

	protected:
		/**
		 * Write the pipeline cache data to an external file.
		 * If the file does not exist, this creates a new file. The file name is "<pipeline name>.fpc". The FPC extension is Flint Pipeline Cache.
		 *
		 * @param size: The size of the data block.
		 * @param pData: The data to be written.
		 */
		void WriteDataToCacheFile(UI64 size, unsigned char* pData) const;

		/**
		 * Read the pipeline cache data from an external file.
		 *
		 * @return The pair of size and data.
		 */
		std::pair<UI64, unsigned char*> ReadDataFromCacheFile() const;

	protected:
		std::string mPipelineName = "";
		bool bShouldPrepareResources = true;
	};
}