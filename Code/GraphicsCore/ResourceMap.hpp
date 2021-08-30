// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Buffer.hpp"
#include "ImageSampler.hpp"

namespace Flint
{
	class GraphicsPipeline;

	/**
	 * Flint resource map object.
	 * This object is used to map resources with its corresponding resource objects in the shader using its name.
	 */
	class ResourceMap
	{
		friend GraphicsPipeline;

	public:
		/**
		 * Construct the resource map using the resource names.
		 *
		 * @param bufferResourceNames: The buffer resource names in the map.
		 * @param imageResourceNames: The image resource names in the map.
		 */
		ResourceMap(const std::vector<std::string>& bufferResourceNames, const std::vector<std::string>& imageResourceNames);

		/**
		 * Set a buffer resource to the map.
		 *
		 * @param name: The name of the resource to bind to.
		 * @param pBuffer: The buffer pointer.
		 */
		void SetResource(const std::string& name, const std::shared_ptr<Buffer>& pBuffer);

		/**
		 * Get a buffer resource from the map.
		 *
		 * @param name: The name of the buffer in the shader.
		 * @return The buffer pointer.
		 */
		const std::shared_ptr<Buffer> GetBufferResource(const std::string& name) const { return pBufferMap.at(name); }

		/**
		 * Get the buffer resource map.
		 *
		 * @return The resource map.
		 */
		const std::unordered_map<std::string, std::shared_ptr<Buffer>> GetBufferResourceMap() const { return pBufferMap; }

		/**
		 * Set an image resource to the map.
		 *
		 * @param name: THe name of the resource to bind to.
		 * @param pSampler: The image sampler pointer.
		 * @param pImage: The image pointer.
		 */
		void SetResource(const std::string& name, const std::shared_ptr<ImageSampler>& pSampler, const std::shared_ptr<Image>& pImage);

		/**
		 * Get an image resource from the map.
		 *
		 * @param name: The name of the image in the shader.
		 * @return The specification and image pointer.
		 */
		const std::pair<std::shared_ptr<ImageSampler>, std::shared_ptr<Image>> GetImageResource(const std::string& name) const { return pImageMap.at(name); }

		/**
		 * Get the image resource map.
		 *
		 * @return The resource map.
		 */
		const std::unordered_map<std::string, std::pair<std::shared_ptr<ImageSampler>, std::shared_ptr<Image>>> GetImageResourceMap() const { return pImageMap; }

	public:
		/**
		 * Is equal to operator.
		 *
		 * @param other: The other resource map.
		 * @return Boolean value.
		 */
		bool operator==(const ResourceMap& other) const;

	protected:
		std::unordered_map<std::string, std::shared_ptr<Buffer>> pBufferMap;
		std::unordered_map<std::string, std::pair<std::shared_ptr<ImageSampler>, std::shared_ptr<Image>>> pImageMap;
	};
}