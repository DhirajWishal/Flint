// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Buffer.hpp"
#include "ImageSampler.hpp"
#include "Shader.hpp"

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

		/**
		 * Buffer binding structure.
		 */
		struct BufferBinding
		{
			BufferBinding() = default;
			BufferBinding(const std::shared_ptr<Buffer>& pBuffer, const UI64 offset) : pBuffer(pBuffer), mOffset(offset) {}

			std::shared_ptr<Buffer> pBuffer = nullptr;
			UI64 mOffset = 0;

			bool operator==(const BufferBinding& other) const { return pBuffer == other.pBuffer && mOffset == other.mOffset; }
		};

		/**
		 * Image binding structure.
		 */
		struct ImageBinding
		{
			ImageBinding() = default;
			ImageBinding(const std::shared_ptr<ImageSampler>& pImageSampler, const std::shared_ptr<Image>& pImage) : pImageSampler(pImageSampler), pImage(pImage) {}

			std::shared_ptr<ImageSampler> pImageSampler = nullptr;
			std::shared_ptr<Image> pImage = nullptr;
			UI64 mViewIndex = 0;

			bool operator==(const ImageBinding& other) const { return pImageSampler == other.pImageSampler && pImage == other.pImage && mViewIndex == other.mViewIndex; }
		};

	public:
		/**
		 * Construct the resource map using the resource names.
		 *
		 * @param bufferResourceKeys: The buffer resource bindings in the map.
		 * @param imageResourceKeys: The image resource bindings in the map.
		 */
		ResourceMap(const std::vector<ShaderResourceKey>& bufferResourceKeys, const std::vector<ShaderResourceKey>& imageResourceKeys);

		/**
		 * Set a buffer resource to the map.
		 *
		 * @param key: The key of the resource to bind to.
		 * @param pBuffer: The buffer pointer.
		 * @param offset: The offset of the buffer. Default is 0.
		 */
		void SetResource(const ShaderResourceKey& key, const std::shared_ptr<Buffer>& pBuffer, const UI64 offset = 0);

		/**
		 * Get a buffer resource from the map.
		 *
		 * @param key: The key of the buffer in the shader.
		 * @return The buffer pointer.
		 */
		const BufferBinding GetBufferResource(const ShaderResourceKey& key) const { return mBufferMap.at(key); }

		/**
		 * Get the buffer resource map.
		 *
		 * @return The resource map.
		 */
		const std::unordered_map<ShaderResourceKey, BufferBinding> GetBufferResourceMap() const { return mBufferMap; }

		/**
		 * Set an image resource to the map.
		 *
		 * @param key: The key of the resource to bind to.
		 * @param pSampler: The image sampler pointer.
		 * @param pImage: The image pointer.
		 */
		void SetResource(const ShaderResourceKey& key, const std::shared_ptr<ImageSampler>& pSampler, const std::shared_ptr<Image>& pImage);

		/**
		 * Get an image resource from the map.
		 *
		 * @param key: The key of the image in the shader.
		 * @return The specification and image pointer.
		 */
		const ImageBinding GetImageResource(const ShaderResourceKey& key) const { return mImageMap.at(key); }

		/**
		 * Get the image resource map.
		 *
		 * @return The resource map.
		 */
		const std::unordered_map<ShaderResourceKey, ImageBinding> GetImageResourceMap() const { return mImageMap; }

	public:
		/**
		 * Is equal to operator.
		 *
		 * @param other: The other resource map.
		 * @return Boolean value.
		 */
		bool operator==(const ResourceMap& other) const;

	protected:
		std::unordered_map<ShaderResourceKey, BufferBinding> mBufferMap = {};
		std::unordered_map<ShaderResourceKey, ImageBinding> mImageMap = {};
	};
}