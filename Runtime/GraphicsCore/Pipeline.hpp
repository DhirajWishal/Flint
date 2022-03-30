// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
#if defined FLINT_PLATFORM_WINDOWS
	constexpr const char* CacheDirectory = "\\Flint\\Cache\\";

#elif defined FLINT_PLATFORM_LINUX
	constexpr const char* CacheDirectory = "/Flint/Cache/";

#endif // defined FLINT_PLATFORM_WINDOWS

	/**
	 * Flint pipeline object.
	 * This object is used to store information about a rendering or compute pipeline.
	 */
	template<class DeviceT, class RenderTargetT, class ResourcePackageT>
	class Pipeline : public DeviceBoundObject<DeviceT>
	{
		friend RenderTargetT;

	public:
		/**
		 * Default constructor.
		 *
		 * @param pDevice The device pointer.
		 * @param pipelineName The name of the pipeline. This name is given to the pipeline cache object created upon destruction. Make sure that this name is unique.
		 *	If you wish to not save cache externally, keep this field empty ("").
		 */
		explicit Pipeline(DeviceT* pDevice, const std::string& pipelineName) : DeviceBoundObject(pDevice), m_PipelineName(pipelineName) {}

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
		void WriteDataToCacheFile(const uint64_t size, unsigned char* pData) const
		{
			// Return if the name is empty.
			if (m_PipelineName.empty())
				return;

			std::ofstream cacheFile(std::filesystem::current_path().string() + CacheDirectory + m_PipelineName + ".fpc", std::ios::out | std::ios::binary);

			if (!cacheFile.is_open())
				throw std::runtime_error("Failed to write cache data!");

			cacheFile.write(reinterpret_cast<char*>(pData), size);
			cacheFile.flush();
			cacheFile.close();
		}

		/**
		 * Read the pipeline cache data from an external file.
		 *
		 * @return The pair of size and data.
		 */
		std::pair<uint64_t, unsigned char*> ReadDataFromCacheFile() const
		{
			// Return if the name is empty.
			if (m_PipelineName.empty())
				return std::pair<uint64_t, unsigned char*>(0, nullptr);

			std::ifstream cacheFile(std::filesystem::current_path().string() + CacheDirectory + m_PipelineName + ".fpc", std::ios::in | std::ios::ate | std::ios::binary);

			// If file does not exist, return without an issue.
			if (!cacheFile.is_open())
				return std::pair<uint64_t, unsigned char*>(0, nullptr);

			const uint64_t size = cacheFile.tellg();
			cacheFile.seekg(0);

			unsigned char* pDataStore = new unsigned char[size];
			cacheFile.read(reinterpret_cast<char*>(pDataStore), size);

			cacheFile.close();
			return std::pair<uint64_t, unsigned char*>(size, pDataStore);
		}

	public:
		/**
		 * Create a new resource package.
		 * 
		 * @param index The set index.
		 * @return The package.
		 */
		std::shared_ptr<ResourcePackageT> CreateResourcePackage(const uint64_t index)
		{
			if (pResourcePackagers.empty())
				CreateResourcePackagers();

			return pResourcePackagers[index]->CreatePackage();
		}

	protected:
		std::string m_PipelineName = "";
		std::vector<std::shared_ptr<ResourcePackageT>> pResourcePackagers = {};

		bool bShouldPrepareResources = true;
	};
}