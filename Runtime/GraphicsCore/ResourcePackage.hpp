// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Buffer.hpp"
#include "ImageSampler.hpp"
#include "ImageView.hpp"

namespace Flint
{
	class ResourcePackager;

	/**
	 * Buffer binding structure.
	 */
	struct BufferBinding
	{
		BufferBinding() = default;
		BufferBinding(const std::shared_ptr<Buffer>& pBuffer, const uint64_t offset) : pBuffer(pBuffer), mOffset(offset) {}

		std::shared_ptr<Buffer> pBuffer = nullptr;
		uint64_t mOffset = 0;

		const bool operator==(const BufferBinding& other) const { return pBuffer == other.pBuffer && mOffset == other.mOffset; }
	};

	/**
	 * Image binding structure.
	 */
	struct ImageBinding
	{
		ImageBinding() = default;
		ImageBinding(const std::shared_ptr<ImageSampler>& pImageSampler, const std::shared_ptr<ImageView>& pImage, const ImageUsage usage)
			: pImageSampler(pImageSampler), pImageView(pImage), mUsage(usage) {}
		ImageBinding(const std::shared_ptr<Image>& pImage, const std::shared_ptr<ImageView>& pImageView, const std::shared_ptr<ImageSampler>& pImageSampler, const ImageUsage usage)
			: pImage(pImage), pImageView(pImageView), pImageSampler(pImageSampler), mUsage(usage) {}

		std::shared_ptr<Image> pImage = nullptr;
		std::shared_ptr<ImageView> pImageView = nullptr;
		std::shared_ptr<ImageSampler> pImageSampler = nullptr;

		uint64_t mViewIndex = 0;
		ImageUsage mUsage = ImageUsage::Graphics;

		const bool operator==(const ImageBinding& other) const { return pImage == other.pImage && pImageView == other.pImageView && pImageSampler == other.pImageSampler && mViewIndex == other.mViewIndex; }
	};

	/**
	 * Flint resource package object.
	 * All the resources that are to be submitted to the pipeline at draw time should be submitted to a package.
	 *
	 * If multiple resources are added for the same binding, we assume that the shader requires multiple of them. The index will depend on the binding order.
	 */
	class ResourcePackage : public FObject
	{
	public:
		/**
		 * Construct the package.
		 *
		 * @param pPackager The packager to which this is bound to.
		 * @param bufferBindings The buffer bindings.
		 * @param imageBindings The image bindings.
		 */
		ResourcePackage(const std::shared_ptr<ResourcePackager>& pPackager, const std::vector<uint32_t>& bufferBindings, const std::vector<uint32_t>& imageBindings);

		/**
		 * Prepare the backend resources if necessary.
		 */
		virtual void PrepareIfNecessary() = 0;

		/**
		 * Bind resources to the package.
		 *
		 * @param binding The binding to bind to.
		 * @param pBuffer The buffer pointer to bind.
		 * @param offset The offset of the buffer to bind. Default is 0.
		 */
		void BindResource(const uint32_t binding, const std::shared_ptr<Buffer>& pBuffer, const uint64_t offset = 0);

		/**
		 * Bind resources to the package.
		 *
		 * @param binding The binding to bind to.
		 * @param pImage The image pointer.
		 * @param pImageView The image view pointer.
		 * @param pImageSampler The image sampler pointer.
		 * @param usage The image usage. This determines what the image usage would be upon binging to the pipeline. Default is Graphics.
		 */
		void BindResource(const uint32_t binding, const std::shared_ptr<Image>& pImage, const std::shared_ptr<ImageView>& pImageView, const std::shared_ptr<ImageSampler>& pImageSampler, const ImageUsage usage = ImageUsage::Graphics);

		/**
		 * Clear the buffer resources.
		 */
		void ClearBufferResources();

		/**
		 * Clear the buffer resources.
		 */
		void ClearImageResources();

	public:
		/**
		 * Get the buffer binding map.
		 *
		 * @return The binding map.
		 */
		const std::unordered_map<uint32_t, std::vector<BufferBinding>> GetBufferBindingMap() const { return mBufferBindings; }

		/**
		 * Get a buffer bindings.
		 *
		 * @param binding The binding of the buffer binding.
		 * @return The buffer bindings.
		 */
		const std::vector<BufferBinding> GetBufferBindings(const uint32_t binding) const;

		/**
		 * Get the image binding map.
		 *
		 * @return The binding map.
		 */
		const std::unordered_map<uint32_t, std::vector<ImageBinding>> GetImageBindingMap() const { return mImageBindings; }

		/**
		 * Get a image bindings.
		 *
		 * @param binding The binding of the image binding.
		 * @return The image bindings.
		 */
		const std::vector<ImageBinding> GetImageBindings(const uint32_t binding) const;

	protected:
		std::shared_ptr<ResourcePackager> pPackager = nullptr;

		std::unordered_map<uint32_t, std::vector<BufferBinding>> mBufferBindings = {};
		std::unordered_map<uint32_t, std::vector<ImageBinding>> mImageBindings = {};

		bool bIsUpdated = true;
	};
}