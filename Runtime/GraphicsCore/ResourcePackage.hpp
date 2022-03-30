// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Buffer.hpp"
#include "ImageSampler.hpp"
#include "ImageView.hpp"

namespace Flint
{
	/**
	 * Buffer binding structure.
	 */
	template<class BufferT>
	struct BufferBinding
	{
		BufferBinding() = default;
		explicit BufferBinding(const BufferT* pBuffer, const uint64_t offset) : pBuffer(pBuffer), m_Offset(offset) {}

		const BufferT* pBuffer = nullptr;
		uint64_t m_Offset = 0;

		const bool operator==(const BufferBinding& other) const { return pBuffer == other.pBuffer && m_Offset == other.m_Offset; }
	};

	/**
	 * Image binding structure.
	 */
	template<class ImageT, class ImageViewT, class ImageSamplerT>
	struct ImageBinding
	{
		ImageBinding() = default;
		explicit ImageBinding(const ImageSamplerT* pImageSampler, const ImageViewT* pImageView, const ImageUsage usage) : pImageSampler(pImageSampler), pImageView(pImageView), m_Usage(usage) {}
		explicit ImageBinding(const ImageT* pImage, const ImageViewT* pImageView, const ImageSamplerT* pImageSampler, const ImageUsage usage) : pImage(pImage), pImageView(pImageView), pImageSampler(pImageSampler), m_Usage(usage) {}

		const Image* pImage = nullptr;
		const ImageViewT* pImageView = nullptr;
		const ImageSamplerT* pImageSampler = nullptr;

		uint64_t m_ViewIndex = 0;
		ImageUsage m_Usage = ImageUsage::Graphics;

		const bool operator==(const ImageBinding& other) const { return pImage == other.pImage && pImageView == other.pImageView && pImageSampler == other.pImageSampler && m_ViewIndex == other.m_ViewIndex; }
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
		explicit ResourcePackage(const std::shared_ptr<ResourcePackagerT>& pPackager, const std::vector<uint32_t>& bufferBindings, const std::vector<uint32_t>& imageBindings)
			: pPackager(pPackager)
		{
			for (const uint32_t binding : bufferBindings)
				m_BufferBindings[binding] = {};

			for (const uint32_t binding : imageBindings)
				m_ImageBindings[binding] = {};
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
			if (m_BufferBindings.find(binding) == m_BufferBindings.end())
				throw std::invalid_argument("Submitted binding is not valid!");

			m_BufferBindings[binding].emplace_back(BufferBindingT(pBuffer, offset));
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
			if (m_ImageBindings.find(binding) == m_ImageBindings.end())
				throw std::invalid_argument("Submitted binding is not valid!");

			m_ImageBindings[binding].emplace_back(ImageBindingT(pImage, pImageView, pImageSampler, usage));
			bIsUpdated = true;
		}

		/**
		 * Clear the buffer resources.
		 */
		void ClearBufferResources()
		{
			for (auto& [binding, resources] : m_BufferBindings)
				resources.clear();
		}

		/**
		 * Clear the buffer resources.
		 */
		void ClearImageResources()
		{
			for (auto& [binding, resources] : m_ImageBindings)
				resources.clear();
		}

	public:
		/**
		 * Get the buffer binding map.
		 *
		 * @return The binding map.
		 */
		const std::unordered_map<uint32_t, std::vector<BufferBindingT>> GetBufferBindingMap() const { return m_BufferBindings; }

		/**
		 * Get a buffer bindings.
		 *
		 * @param binding The binding of the buffer binding.
		 * @return The buffer bindings.
		 */
		const std::vector<BufferBindingT> GetBufferBindings(const uint32_t binding) const
		{
			if (m_BufferBindings.find(binding) == m_BufferBindings.end())
				throw std::invalid_argument("Submitted binding is not valid!");

			return m_BufferBindings.at(binding);
		}

		/**
		 * Get the image binding map.
		 *
		 * @return The binding map.
		 */
		const std::unordered_map<uint32_t, std::vector<ImageBindingT>> GetImageBindingMap() const { return m_ImageBindings; }

		/**
		 * Get a image bindings.
		 *
		 * @param binding The binding of the image binding.
		 * @return The image bindings.
		 */
		const std::vector<ImageBindingT> GetImageBindings(const uint32_t binding) const
		{
			if (m_ImageBindings.find(binding) == m_ImageBindings.end())
				throw std::invalid_argument("Submitted binding is not valid!");

			return m_ImageBindings.at(binding);
		}

	protected:
		std::shared_ptr<ResourcePackagerT> pPackager = nullptr;

		std::unordered_map<uint32_t, std::vector<BufferBindingT>> m_BufferBindings = {};
		std::unordered_map<uint32_t, std::vector<ImageBindingT>> m_ImageBindings = {};

		bool bIsUpdated = true;
	};
}