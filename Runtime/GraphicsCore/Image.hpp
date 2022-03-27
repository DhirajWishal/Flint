// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Buffer.hpp"

namespace Flint
{
	class ImageView;

	/**
	 * Loading operation enum.
	 */
	enum class LoadOperation : uint8_t {
		Store,
		Load,
		DontCare
	};

	/**
	 * Image render target specifications.
	 * This enum contains a list of flags which can be used to define how the image should behave when used as an render target attachment.
	 *
	 * If not set to anything, meaning 0, the default will be set. But if you wish to set to any other, make sure to follow the following guide lines.
	 * The operations can be divided into two main sections.
	 * 1. Color image section.
	 * 2. Stencil image section.
	 *
	 * These sections contain two more sub sections,
	 * 1. Load operation.
	 * 2. Store operation.
	 *
	 * The user cannot define two flags within the same sub section. In that case the lowest (first) flag will be used.
	 */
	enum class ImageRenderTargetSpecification : uint16_t {
		// Color image section.
		// Defines that we load data upon attachment load.
		LoadOnLoad = BitShiftLeft(0),

		// Defines that we clear the image attachment upon load. Make sure to have a clear value.
		ClearOnLoad = BitShiftLeft(1),

		// Defines that we do not care about the previous content of the image.
		DiscardLoad = BitShiftLeft(2),

		// Defines that we store the data upon attachment store.
		StoreOnStore = BitShiftLeft(3),

		// Defines that we don't care about the storing operation.
		DiscardStore = BitShiftLeft(4),

		// Stencil image section.
		// Defines that we load data upon stencil load.
		StencilLoadOnLoad = BitShiftLeft(5),

		// Defines that we clear the image stencil upon load. Make sure to have a clear value.
		StencilClearOnLoad = BitShiftLeft(6),

		// Defines that we do not care about the previous content of the stencil.
		StencilDiscardLoad = BitShiftLeft(7),

		// Defines that we store the data upon stencil store.
		StencilStoreOnStore = BitShiftLeft(8),

		// Defines that we don't care about the storing operation.
		StencilDiscardStore = BitShiftLeft(9),
	};

	constexpr bool operator&(const ImageRenderTargetSpecification lhs, const ImageRenderTargetSpecification rhs) { return static_cast<uint16_t>(lhs) & static_cast<uint16_t>(rhs); }
	constexpr ImageRenderTargetSpecification operator|(const ImageRenderTargetSpecification lhs, const ImageRenderTargetSpecification rhs) { return static_cast<ImageRenderTargetSpecification>(static_cast<uint16_t>(lhs) | static_cast<uint16_t>(rhs)); }

	/**
	 * Image render target specification structure.
	 * This structure is used to specify how the image behaves when used as an render target attachment.
	 *
	 * If the image is not intended to use as a render target attachment, or uses the default attachment specification, editing this structure is not necessary.
	 */
	 //struct ImageRenderingTargetSpecification
	 //{
	 //	LoadOperation mLoadOperation = LoadOperation::DontCare;
	 //	LoadOperation mStencilLoadOperation = LoadOperation::DontCare;
	 //
	 //	bool bShouldStoreOnStore = false;
	 //	bool bShouldStoreOnStencilStore = false;
	 //};

	 /**
	  * Flint image object.
	  * This object is used to store information about a single image. This can have multiple levels of the same image.
	  */
	class Image : public DeviceBoundObject
	{
	public:
		/**
		 * Construct the image.
		 *
		 * @param pDevice The device pointer.
		 * @param type The image type.
		 * @param usage The image usage.
		 * @param extent The image extent.
		 * @param format The image format.
		 * @param layers The number of layers in the image.
		 * @param mipLevels The mip levels.
		 * @param pImageData The image data pointer to load data from.
		 * @param sampleCount The multi sample count to use.
		 */
		Image(const std::shared_ptr<Device>& pDevice, const ImageType type, const ImageUsage usage, const FBox3D& extent, const PixelFormat format, const uint8_t layers, const uint32_t mipLevels, const void* pImageData, const MultiSampleCount sampleCount = MultiSampleCount::One);

		/**
		 * Generate mip maps.
		 */
		virtual void GenerateMipMaps() = 0;

		/**
		 * Copy the image content to a buffer.
		 *
		 * @return The stagging buffer.
		 */
		virtual std::shared_ptr<Buffer> CopyToBuffer() = 0;

		/**
		 * Create a new image view.
		 *
		 * @param baseLayerIndex The base layer index to start covering from.
		 * @param layerCount The number of layers for the image view to cover.
		 * @param baseMipLevel The base mip level to start from.
		 * @param mipLevels The mip levels to cover.
		 * @param usage The image usage.
		 */
		virtual std::shared_ptr<ImageView> CreateImageView(const uint32_t baseLayerIndex, const uint32_t layerCount, const uint32_t baseMipLevel, const uint32_t mipLevels, const ImageUsage usage) = 0;

		/**
		 * Get the value of a single pixel.
		 *
		 * @tparam Type: The type of the pixel.
		 * @param position The position of the pixel.
		 * @return The pixel data.
		 */
		template<class Type>
		Type GetPixelValue(const FBox3D position)
		{
			std::shared_ptr<Buffer> pStagingBuffer = CopyToBuffer();
			Type* pPixels = static_cast<Type*>(pStagingBuffer->MapMemory(pStagingBuffer->GetSize()));

			uint64_t row = static_cast<uint64_t>(position.mHeight) * mExtent.mWidth;
			uint64_t index = row + position.mWidth;
			Type pixel = pPixels[index];

			pStagingBuffer->UnmapMemory();
			pStagingBuffer->Terminate();

			return pixel;
		}

	public:
		/**
		 * Get the extent of the image.
		 *
		 * @return The extent.
		 */
		const FBox3D GetExtent() const { return mExtent; }

		/**
		 * Get the mip levels of the current image.
		 *
		 * @return The mip levels.
		 */
		const uint32_t GetMipLevels() const { return mMipLevels; }

		/**
		 * Get the type of the image.
		 *
		 * @return The type.
		 */
		const ImageType GetType() const { return mType; }

		/**
		 * Get the image usage.
		 *
		 * @return The usage.
		 */
		const ImageUsage GetUsage() const { return mUsage; }

		/**
		 * Get the format of the image.
		 *
		 * @return The format.
		 */
		const PixelFormat GetFormat() const { return mFormat; }

		/**
		 * Get the layer count of the image.
		 *
		 * @return The layer count.
		 */
		const uint8_t GetLayerCount() const { return mLayerCount; }

		/**
		 * Get the image render target specification structure.
		 *
		 * @return The image render target specification structure.
		 */
		ImageRenderTargetSpecification& GetImageRenderTargetSpecification() { return mImageRenderingTargetSpecification; }

		/**
		 * Get the image render target specification structure.
		 *
		 * @return The image render target specification structure.
		 */
		const ImageRenderTargetSpecification GetImageRenderTargetSpecification() const { return mImageRenderingTargetSpecification; }

	public:
		/**
		 * Get the best mip levels for an image.
		 *
		 * @param extent The extent of the image.
		 * @return The mip levels.
		 */
		static uint32_t GetBestMipLevels(const FBox3D extent);

	protected:
		FBox3D mExtent = {};
		uint32_t mMipLevels = 0;
		ImageType mType = ImageType::TwoDimension;
		ImageUsage mUsage = ImageUsage::Graphics;
		PixelFormat mFormat = PixelFormat::Undefined;
		MultiSampleCount mMultiSampleCount = MultiSampleCount::One;
		uint8_t mLayerCount = 1;

		ImageRenderTargetSpecification mImageRenderingTargetSpecification = ImageRenderTargetSpecification(0);
	};
}