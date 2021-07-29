// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Buffer.hpp"

#include <unordered_map>

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
		 */
		ResourceMap(const std::vector<std::string>& bufferResourceNames);

		/**
		 * Set a resource to the map.
		 *
		 * @param name: The name of the resource to bind to.
		 * @param pBuffer: The buffer pointer.
		 * @return Boolean value stating if it was successfully bound or not.
		 */
		bool SetResource(const std::string& name, const std::shared_ptr<Buffer>& pBuffer);

		/**
		 * Get a buffer resource from the map.
		 *
		 * @param name: The name of the buffer in the shader.
		 * @return The buffer pointer.
		 */
		std::shared_ptr<Buffer> GetBufferResource(const std::string& name) const { return pBufferMap.at(name); }

		/**
		 * Get the buffer resource map.
		 *
		 * @return The resource map.
		 */
		const std::unordered_map<std::string, std::shared_ptr<Buffer>> GetBufferResourceMap() const { return pBufferMap; }

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
	};
}