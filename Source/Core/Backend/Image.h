// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Device.h"
#include "Core\Hasher\Hasher.h"

#define FLINT_DEFAULT_CHANNEL_COUNT		4
#define FLINT_DEFAULT_BPP				32

#define FLINT_CREATE_FORMAT_ID(r, g, b, a)		((static_cast<UI8>(r) << 24) | (static_cast<UI8>(g) << 16) | (static_cast<UI8>(b) << 8) | static_cast<UI8>(a))

namespace Flint
{
	namespace Backend
	{
		/**
		 * Image usage enum.
		 * This defines what the image object is used for.
		 */
		enum class ImageUsage : UI8 {
			UNDEFINED,
			GRAPHICS_1D, GRAPHICS_2D, GRAPHICS_3D, GRAPHICS_CUBEMAP,
			STORAGE_1D, STORAGE_2D, STORAGE_3D, STORAGE_CUBEMAP,
		};

		/**
		 * Image format enum.
		 */
		enum class ImageFormat : UI8 {
			UNDEFINED,						// Undefined format

			RG_4_UNORMAL_PACK8, RGBA_4_UNORMAL_PACK16, BGRA_4_UNORMAL_PACK16, R5G6B5_UNORMAL_PACK16,
			B5G6R5_UNORMAL_PACK16, R5G6B5A1_UNORMAL_PACK16, B5G6R5A1_UNORMAL_PACK16, A1B5G6R5_UNORMAL_PACK16,

			R_8_UNORMAL, R_8_SNORMAL, R_8_USCALED,
			R_8_SSCALED, R_8_UINT, R_8_SINT, R_8_SRGB,

			RG_8_UNORMAL, RG_8_SNORMAL, RG_8_USCALED,
			RG_8_SSCALED, RG_8_UINT, RG_8_SINT, RG_8_SRGB,

			RGB_8_UNORMAL, RGB_8_SNORMAL, RGB_8_USCALED, RGB_8_SSCALED,
			RGB_8_UINT, RGB_8_SINT, RGB_8_SRGB,

			BGR_8_UNORMAL, BGR_8_SNORMAL, BGR_8_USCALED,
			BGR_8_SSCALED, BGR_8_UINT, BGR_8_SINT, BGR_8_SRGB,

			RGBA_8_UNORMAL, RGBA_8_SNORMAL, RGBA_8_USCALED,
			RGBA_8_SSCALED, RGBA_8_UINT, RGBA_8_SINT, RGBA_8_SRGB,

			BGRA_8_UNORMAL, BGRA_8_SNORMAL, BGRA_8_USCALED,
			BGRA_8_SSCALED, BGRA_8_UINT, BGRA_8_SINT, BGRA_8_SRGB,

			RGBA_8_UNORMAL_PACK32, RGBA_8_SNORMAL_PACK32, RGBA_8_USCALED_PACK32,
			RGBA_8_SSCALED_PACK32, RGBA_8_UINT_PACK32, RGBA_8_SINT_PACK32, RGBA_8_SRGB_PACK32,

			A2R10G10B10_UNORMAL_PACK32, A2R10G10B10_SNORMAL_PACK32, A2R10G10B10_USCALED_PACK32,
			A2R10G10B10_SSCALED_PACK32, A2R10G10B10_UINT_PACK32, A2R10G10B10_SINT_PACK32,

			A2B10G10R10_UNORMAL_PACK32, A2B10G10R10_SNORMAL_PACK32, A2B10G10R10_USCALED_PACK32,
			A2B10G10R10_SSCALED_PACK32, A2B10G10R10_UINT_PACK32, A2B10G10R10_SINT_PACK32,

			R_16_UNORMAL, R_16_SNORMAL, R_16_USCALED,
			R_16_SSCALED, R_16_UINT, R_16_SINT, R_16_SFLOAT,

			RG_16_UNORMAL, RG_16_SNORMAL, RG_16_USCALED,
			RG_16_SSCALED, RG_16_UINT, RG_16_SINT, RG_16_SFLOAT,

			RGB_16_UNORMAL, RGB_16_SNORMAL, RGB_16_USCALED,
			RGB_16_SSCALED, RGB_16_UINT, RGB_16_SINT, RGB_16_SFLOAT,

			RGBA_16_UNORMAL, RGBA_16_SNORMAL, RGBA_16_USCALED,
			RGBA_16_SSCALED, RGBA_16_UINT, RGBA_16_SINT, RGBA_16_SFLOAT,

			R_32_UINT, R_32_SINT, R_32_SFLOAT,
			RG_32_UINT, RG_32_SINT, RG_32_SFLOAT,
			RGB_32_UINT, RGB_32_SINT, RGB_32_SFLOAT,
			RGBA_32_UINT, RGBA_32_SINT, RGBA_32_SFLOAT,

			R_64_UINT, R_64_SINT, R_64_SFLOAT,
			RG_64_UINT, RG_64_SINT, RG_64_SFLOAT,
			RGB_64_UINT, RGB_64_SINT, RGB_64_SFLOAT,
			RGBA_64_UINT, RGBA_64_SINT, RGBA_64_SFLOAT,

			B10G11R11_UFLOAT_PACK32, E5B9G9R9_UFLOAT_PACK32,

			D_16_UNORM, X8_D_24_UNORM_PACK32, D_32_SFLOAT,
			S_8_UINT, D_16_UNORM_S_8_UINT, D_24_UNORM_S_8_UINT, D_32_SFLOAT_S_8_UINT,

			BC1_RGB_UNORM_BLOCK, BC1_RGB_SRGB_BLOCK,
			BC1_RGBA_UNORM_BLOCK, BC1_RGBA_SRGB_BLOCK,
			BC2_UNORM_BLOCK, BC2_SRGB_BLOCK,
			BC3_UNORM_BLOCK, BC3_SRGB_BLOCK,
			BC4_UNORM_BLOCK, BC4_SNORM_BLOCK,
			BC5_UNORM_BLOCK, BC5_SNORM_BLOCK,
			BC6H_UFLOAT_BLOCK, BC6H_SFLOAT_BLOCK,
			BC7_UNORM_BLOCK, BC7_SRGB_BLOCK,

			ETC2_RGB_8_UNORM_BLOCK, ETC2_RGB_8_SRGB_BLOCK,
			ETC2_R8G8B8A1_UNORM_BLOCK, ETC2_R8G8B8A1_SRGB_BLOCK,
			ETC2_RGBA_8_UNORM_BLOCK, ETC2_RGBA_8_SRGB_BLOCK,

			EAC_R_11_UNORM_BLOCK, EAC_R_11_SNORM_BLOCK,
			EAC_RG_11_UNORM_BLOCK, EAC_RG_11_SNORM_BLOCK,

			ASTC_4x4_UNORM_BLOCK, ASTC_4x4_SRGB_BLOCK,
			ASTC_5x4_UNORM_BLOCK, ASTC_5x4_SRGB_BLOCK,
			ASTC_5x5_UNORM_BLOCK, ASTC_5x5_SRGB_BLOCK,
			ASTC_6x5_UNORM_BLOCK, ASTC_6x5_SRGB_BLOCK,
			ASTC_6x6_UNORM_BLOCK, ASTC_6x6_SRGB_BLOCK,
			ASTC_8x5_UNORM_BLOCK, ASTC_8x5_SRGB_BLOCK,
			ASTC_8x6_UNORM_BLOCK, ASTC_8x6_SRGB_BLOCK,
			ASTC_8x8_UNORM_BLOCK, ASTC_8x8_SRGB_BLOCK,
			ASTC_10x5_UNORM_BLOCK, ASTC_10x5_SRGB_BLOCK,
			ASTC_10x6_UNORM_BLOCK, ASTC_10x6_SRGB_BLOCK,
			ASTC_10x8_UNORM_BLOCK, ASTC_10x8_SRGB_BLOCK,
			ASTC_10x10_UNORM_BLOCK, ASTC_10x10_SRGB_BLOCK,
			ASTC_12x10_UNORM_BLOCK, ASTC_12x10_SRGB_BLOCK,
			ASTC_12x12_UNORM_BLOCK, ASTC_12x12_SRGB_BLOCK,
		};

		/**
		 * Image sampler address mode.
		 */
		enum class SamplerAddressMode : UI8 {
			REPEAT,
			MIRRORED_REPEAT,
			CLAMP_TO_EDGE,
			CLAMP_TO_BORDER,
			MIRROR_CLAMP_TO_EDGE,
		};

		/**
		 * Image sampler border color.
		 */
		enum class SamplerBorderColor : UI8 {
			FLOAT_TRANSPARENT_BLACK,
			INT_TRANSPARENT_BLACK,
			FLOAT_OPAQUE_BLACK,
			INT_OPAQUE_BLACK,
			FLOAT_OPAQUE_WHITE,
			INT_OPAQUE_WHITE,
		};

		/**
		 * Image sampler filter.
		 */
		enum class SamplerFilter : UI8 {
			NEAREST,
			LINEAR,
			CUBIC_IMAGE,
		};

		/**
		 * Image sampler specification.
		 * This structure defines how the image sampler should be initialized.
		 */
		struct SamplerSpecification {
			float mMinLOD = 0.0f;
			float mLODBias = 1.0f;

			SamplerAddressMode mAddressModeU = SamplerAddressMode::CLAMP_TO_EDGE;
			SamplerAddressMode mAddressModeV = SamplerAddressMode::CLAMP_TO_EDGE;
			SamplerAddressMode mAddressModeW = SamplerAddressMode::CLAMP_TO_EDGE;

			SamplerBorderColor mBorderColor = SamplerBorderColor::FLOAT_OPAQUE_BLACK;

			SamplerFilter mFilter = SamplerFilter::LINEAR;

			float mMipLevel = 0.0f;

			const bool operator==(const SamplerSpecification& other) const;
		};

		/**
		 * Flint image object.
		 * This object is the base class for all the backend image objects. Image objects are used to store image data in the GPU.
		 *
		 * @tparam TDevice: The device type.
		 */
		template<class TDevice>
		class Image : public BackendObject {
		public:
			using DeviceType = TDevice;

		public:
			Image() {}
			virtual ~Image() {}

			/**
			 * Initialize the image object.
			 *
			 * @param pDevice: The device pointer which the image is bound to.
			 * @param width: The width of the image.
			 * @param height: The height of the image.
			 * @param depth: The depth of the image.
			 * @param usage: The image usage.
			 * @param bitsPerPixel: The number of bits a pixel contains.
			 * @param layers: The number of image layers the image consists of.
			 */
			virtual void Initialize(DeviceType* pDevice, UI64 width, UI64 height, UI64 depth, ImageUsage usage, UI8 bitsPerPixel = FLINT_DEFAULT_BPP, UI8 layers = 1) = 0;

			/**
			 * Terminate the image.
			 */
			virtual void Terminate() = 0;

			/**
			 * Copy image data to the image.
			 *
			 * @param pData: The data to be copied,
			 * @param width: The width of the source image.
			 * @param widthOffset: The width offset of the destination (this) image to be copied to.
			 * @param height: The height of the source image.
			 * @param heightOffset: The height offset of the destination (this) image to be copied to.
			 * @param depth: The depth of the source image.
			 * @param depthOffset: The depth offset of the destination (this) image to be copied to.
			 * @param bitsPerPixel: The BPP of the source image.
			 */
			virtual void CopyData(unsigned char* pData, UI64 width, UI64 widthOffset, UI64 height, UI64 heightOffset, UI64 depth, UI64 depthOffset, UI8 bitsPerPixel = FLINT_DEFAULT_BPP) = 0;

		protected:
			DeviceType* pDevice = nullptr;
			UI64 mWidth = 0, mHeight = 0, mDepth = 0;
			UI32 mMipLevel = 1;
			UI8 mBitsPerPixel = 8, mLayers = 1;

			ImageUsage mUsage = ImageUsage::UNDEFINED;
		};
	}
}

namespace std
{
	template<>
	struct hash<Flint::Backend::SamplerSpecification> {
		const size_t operator()(const Flint::Backend::SamplerSpecification& spec) const
		{
			return Flint::Hasher::QuickHash64(&spec, sizeof(Flint::Backend::SamplerSpecification));
		}
	};
}