// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Buffer.hpp"
#include "ImageSampler.hpp"

namespace Flint
{
	class ResourcePackager;

	/**
	 * Buffer binding structure.
	 */
	struct BufferBinding
	{
		BufferBinding() = default;
		BufferBinding(const std::shared_ptr<Buffer>& pBuffer, const UI64 index, const UI64 offset) : pBuffer(pBuffer), mArrayIndex(index), mOffset(offset) {}

		std::shared_ptr<Buffer> pBuffer = nullptr;
		UI64 mArrayIndex = 0;
		UI64 mOffset = 0;

		const bool operator==(const BufferBinding& other) const { return pBuffer == other.pBuffer && mOffset == other.mOffset; }
	};

	/**
	 * Image binding structure.
	 */
	struct ImageBinding
	{
		ImageBinding() = default;
		ImageBinding(const std::shared_ptr<ImageSampler>& pImageSampler, const std::shared_ptr<Image>& pImage, const UI64 index) : pImageSampler(pImageSampler), pImage(pImage), mArrayIndex(index) {}

		std::shared_ptr<ImageSampler> pImageSampler = nullptr;
		std::shared_ptr<Image> pImage = nullptr;
		UI64 mViewIndex = 0;
		UI64 mArrayIndex = 0;

		const bool operator==(const ImageBinding& other) const { return pImageSampler == other.pImageSampler && pImage == other.pImage && mViewIndex == other.mViewIndex; }
	};

	/**
	 * Flint resource package object.
	 * All the resources that are to be submitted to the pipeline at draw time should be submitted to a package.
	 */
	class ResourcePackage : public FObject
	{
	public:
		/**
		 * Construct the package.
		 *
		 * @param pPackager: The packager to which this is bound to.
		 * @param bufferBindings: The buffer bindings.
		 * @param imageBindings: The image bindings.
		 */
		ResourcePackage(const std::shared_ptr<ResourcePackager>& pPackager, const std::vector<UI32>& bufferBindings, const std::vector<UI32>& imageBindings);

		/**
		 * Prepare the backend resources if necessary.
		 */
		virtual void PrepareIfNecessary() = 0;

		/**
		 * Bind resources to the package.
		 *
		 * @param binding: The binding to bind to.
		 * @param pBuffer: The buffer pointer to bind.
		 * @param index: The buffer index to bind to. Default is 0.
		 * @param offset: The offset of the buffer to bind. Default is 0.
		 */
		void BindResource(const UI32 binding, const std::shared_ptr<Buffer>& pBuffer, const UI64 index = 0, const UI64 offset = 0);

		/**
		 * Bind resources to the package.
		 *
		 * @param binding: The binding to bind to.
		 * @param pImageSampler: The image sampler pointer.
		 * @param pImage: The image pointer.
		 * @param index: The index of the image to bind to. Default is 0.
		 */
		void BindResource(const UI32 binding, const std::shared_ptr<ImageSampler>& pImageSampler, const std::shared_ptr<Image>& pImage, const UI64 index = 0);

	public:
		/**
		 * Get the buffer binding map.
		 *
		 * @return The binding map.
		 */
		const std::unordered_map<UI32, BufferBinding> GetBufferBindingMap() const { return mBufferBindings; }

		/**
		 * Get a buffer binding.
		 *
		 * @param binding: The binding of the buffer binding.
		 * @return The buffer binding.
		 */
		const BufferBinding GetBufferBinding(const UI32 binding) const;

		/**
		 * Get the image binding map.
		 *
		 * @return The binding map.
		 */
		const std::unordered_map<UI32, ImageBinding> GetImageBindingMap() const { return mImageBindings; }

		/**
		 * Get a image binding.
		 *
		 * @param binding: The binding of the image binding.
		 * @return The image binding.
		 */
		const ImageBinding GetImageBinding(const UI32 binding) const;

	protected:
		std::shared_ptr<ResourcePackager> pPackager = nullptr;

		std::unordered_map<UI32, BufferBinding> mBufferBindings;
		std::unordered_map<UI32, ImageBinding> mImageBindings;

		bool bIsUpdated = true;
	};
}