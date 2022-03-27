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
	template<class BufferT>
	struct BufferBinding
	{
		BufferBinding() = default;
		BufferBinding(const BufferT* pBuffer, const uint64_t offset) : pBuffer(pBuffer), mOffset(offset) {}

		const BufferT* pBuffer = nullptr;
		uint64_t mOffset = 0;

		const bool operator==(const BufferBinding& other) const { return pBuffer == other.pBuffer && mOffset == other.mOffset; }
	};

	/**
	 * Image binding structure.
	 */
	template<class ImageT, class ImageViewT, class ImageSamplerT>
	struct ImageBinding
	{
		ImageBinding() = default;
		ImageBinding(const ImageSamplerT* pImageSampler, const ImageViewT* pImageView, const ImageUsage usage) : pImageSampler(pImageSampler), pImageView(pImageView), mUsage(usage) {}
		ImageBinding(const ImageT* pImage, const ImageViewT* pImageView, const ImageSamplerT* pImageSampler, const ImageUsage usage) : pImage(pImage), pImageView(pImageView), pImageSampler(pImageSampler), mUsage(usage) {}

		const Image* pImage = nullptr;
		const ImageViewT* pImageView = nullptr;
		const ImageSamplerT* pImageSampler = nullptr;

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
	template<class ResourcePackagerT, class BufferT, class ImageT, class ImageViewT, class ImageSamplerT>
	class ResourcePackage : public FObject
	{
	public:
		using BufferBindingT = BufferBinding<BufferT>;
		using ImageBindingT = ImageBinding<ImageT, ImageViewT, ImageSamplerT>;

		/**
		 * Construct the package.
		 *
		 * @param pPackager The packager to which this is bound to.
		 * @param bufferBindings The buffer bindings.
		 * @param imageBindings The image bindings.
		 */
		ResourcePackage(const std::shared_ptr<ResourcePackagerT>& pPackager, const std::vector<uint32_t>& bufferBindings, const std::vector<uint32_t>& imageBindings)
			: pPackager(pPackager)
		{
			for (const uint32_t binding : bufferBindings)
				mBufferBindings[binding] = {};

			for (const uint32_t binding : imageBindings)
				mImageBindings[binding] = {};
		}

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
		void BindResource(const uint32_t binding, const BufferT* pBuffer, const uint64_t offset = 0)
		{
			if (mBufferBindings.find(binding) == mBufferBindings.end())
				throw std::invalid_argument("Submitted binding is not valid!");

			mBufferBindings[binding].emplace_back(BufferBindingT(pBuffer, offset));
			bIsUpdated = true;
		}

		/**
		 * Bind resources to the package.
		 *
		 * @param binding The binding to bind to.
		 * @param pImage The image pointer.
		 * @param pImageView The image view pointer.
		 * @param pImageSampler The image sampler pointer.
		 * @param usage The image usage. This determines what the image usage would be upon binging to the pipeline. Default is Graphics.
		 */
		void BindResource(const uint32_t binding, const ImageT* pImage, const ImageViewT* pImageView, const ImageSamplerT* pImageSampler, const ImageUsage usage = ImageUsage::Graphics)
		{
			if (mImageBindings.find(binding) == mImageBindings.end())
				throw std::invalid_argument("Submitted binding is not valid!");

			mImageBindings[binding].emplace_back(ImageBindingT(pImage, pImageView, pImageSampler, usage));
			bIsUpdated = true;
		}

		/**
		 * Clear the buffer resources.
		 */
		void ClearBufferResources()
		{
			for (auto& [binding, resources] : mBufferBindings)
				resources.clear();
		}

		/**
		 * Clear the buffer resources.
		 */
		void ClearImageResources()
		{
			for (auto& [binding, resources] : mImageBindings)
				resources.clear();
		}

	public:
		/**
		 * Get the buffer binding map.
		 *
		 * @return The binding map.
		 */
		const std::unordered_map<uint32_t, std::vector<BufferBindingT>> GetBufferBindingMap() const { return mBufferBindings; }

		/**
		 * Get a buffer bindings.
		 *
		 * @param binding The binding of the buffer binding.
		 * @return The buffer bindings.
		 */
		const std::vector<BufferBindingT> GetBufferBindings(const uint32_t binding) const
		{
			if (mBufferBindings.find(binding) == mBufferBindings.end())
				throw std::invalid_argument("Submitted binding is not valid!");

			return mBufferBindings.at(binding);
		}

		/**
		 * Get the image binding map.
		 *
		 * @return The binding map.
		 */
		const std::unordered_map<uint32_t, std::vector<ImageBindingT>> GetImageBindingMap() const { return mImageBindings; }

		/**
		 * Get a image bindings.
		 *
		 * @param binding The binding of the image binding.
		 * @return The image bindings.
		 */
		const std::vector<ImageBindingT> GetImageBindings(const uint32_t binding) const
		{
			if (mImageBindings.find(binding) == mImageBindings.end())
				throw std::invalid_argument("Submitted binding is not valid!");

			return mImageBindings.at(binding);
		}

	protected:
		std::shared_ptr<ResourcePackagerT> pPackager = nullptr;

		std::unordered_map<uint32_t, std::vector<BufferBindingT>> mBufferBindings = {};
		std::unordered_map<uint32_t, std::vector<ImageBindingT>> mImageBindings = {};

		bool bIsUpdated = true;
	};
}