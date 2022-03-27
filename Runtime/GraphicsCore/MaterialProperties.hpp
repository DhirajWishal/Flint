// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Image.hpp"

namespace Flint
{
	namespace MaterialProperties
	{
		/**
		 * Material type enum.
		 */
		enum class MaterialType : uint8_t 
		{
			FLOAT,
			FLOAT_2,
			FLOAT_3,
			FLOAT_4,

			DOUBLE,
			DOUBLE_2,
			DOUBLE_3,
			DOUBLE_4,

			TEXTURE_PATH,
			TEXTURE_IMAGE,
			TEXTURE_DATA,
		};

		/**
		 * Flint material property class.
		 * This object is the base class to all the supported properties.
		 */
		class Property : public FObject
		{
		public:
			Property(const MaterialType type) : mType(type) {}

			const MaterialType GetType() const { return mType; }

		protected:
			MaterialType mType = MaterialType::FLOAT;
		};

		/**
		 * Single float property.
		 */
		class Float final : public Property
		{
		public:
			/**
			 * Constructor.
			 *
			 * @param value The value to store.
			 */
			Float(const float value) : Property(MaterialType::FLOAT), mValue(value) {}

			float mValue = 0.0f;
		};

		/**
		 * Two float property.
		 */
		class Float2 final : public Property
		{
		public:
			/**
			 * Constructor.
			 *
			 * @param values The values to store.
			 */
			Float2(const float values[2]) : Property(MaterialType::FLOAT_2)
			{
				mValues[0] = values[0];
				mValues[1] = values[1];
			}

			float mValues[2] = {};
		};

		/**
		 * Three float property.
		 */
		class Float3 final : public Property
		{
		public:
			/**
			 * Constructor.
			 *
			 * @param values The values to store.
			 */
			Float3(const float values[3]) : Property(MaterialType::FLOAT_3)
			{
				mValues[0] = values[0];
				mValues[1] = values[1];
				mValues[2] = values[2];
			}

			float mValues[3] = {};
		};

		/**
		 * Four float property.
		 */
		class Float4 final : public Property
		{
		public:
			/**
			 * Constructor.
			 *
			 * @param values The values to store.
			 */
			Float4(const float values[4]) : Property(MaterialType::FLOAT_4)
			{
				mValues[0] = values[0];
				mValues[1] = values[1];
				mValues[2] = values[2];
				mValues[3] = values[3];
			}

			float mValues[4] = {};
		};

		/**
		 * Single double property.
		 */
		class Double final : public Property
		{
		public:
			/**
			 * Constructor.
			 *
			 * @param value The value to store.
			 */
			Double(const double value) : Property(MaterialType::DOUBLE), mValue(value) {}

			double mValue = 0.0f;
		};

		/**
		 * Two float property.
		 */
		class Double2 final : public Property
		{
		public:
			/**
			 * Constructor.
			 *
			 * @param values The values to store.
			 */
			Double2(const double values[2]) : Property(MaterialType::DOUBLE_2)
			{
				mValues[0] = values[0];
				mValues[1] = values[1];
			}

			double mValues[2] = {};
		};

		/**
		 * Three float property.
		 */
		class Double3 final : public Property
		{
		public:
			/**
			 * Constructor.
			 *
			 * @param values The values to store.
			 */
			Double3(const double values[3]) : Property(MaterialType::DOUBLE_3)
			{
				mValues[0] = values[0];
				mValues[1] = values[1];
				mValues[2] = values[2];
			}

			double mValues[3] = {};
		};

		/**
		 * Four float property.
		 */
		class Double4 final : public Property
		{
		public:
			/**
			 * Constructor.
			 *
			 * @param values The values to store.
			 */
			Double4(const double values[4]) : Property(MaterialType::DOUBLE_4)
			{
				mValues[0] = values[0];
				mValues[1] = values[1];
				mValues[2] = values[2];
				mValues[3] = values[3];
			}

			double mValues[4] = {};
		};

		/**
		 * Texture type enum.
		 */
		enum class TextureType : uint8_t 
		{
			Undefined,
			Specular,
			Diffuse,
			Ambient,
			Emissive,
			Normals,
			Height,
			Shininess,
			Opacity,
			Displacement,
			Lightmap,
			Reflection,
			BaseColor,
			NormalCamera,
			EmissionColor,
			Metalness,
			DiffuseRoughness,
			AmbientOcclusion,
			Transmission
		};

		/**
		 * Texture path property.
		 */
		class TexturePath final : public Property
		{
		public:
			/**
			 * Constructor.
			 *
			 * @param path The path of the texture file.
			 * @param type The texture type.
			 */
			TexturePath(const std::filesystem::path& path, const TextureType type) : Property(MaterialType::TEXTURE_PATH), mTexturePath(path), mType(type) {}

			std::filesystem::path mTexturePath = {};
			TextureType mType = TextureType::Undefined;
		};

		/**
		 * Texture image property.
		 * This object stores information about a single texture image.
		 */
		template<class ImageT>
		class TextureImage final : public Property
		{
		public:
			/**
			 * Constructor.
			 *
			 * @param pImage The image pointer.
			 * @param type The texture type.
			 */
			TextureImage(std::unique_ptr<ImageT>&& pImage, const TextureType type) : Property(MaterialType::TEXTURE_IMAGE), pImage(std::move(pImage)), mType(type) {}

			std::unique_ptr<ImageT> pImage = nullptr;
			TextureType mType = TextureType::Undefined;
		};

		/**
		 * Texture data property.
		 */
		template<class ImageT, class DeviceT>
		class TextureData final : public Property
		{
		public:
			/**
			 * Constructor.
			 *
			 * @param pData The data pointer.
			 * @param extent The image extent.
			 * @param format The image format.
			 * @param type The texture type.
			 */
			TextureData(unsigned char* pData, const FBox3D extent, const PixelFormat format, const TextureType type) : Property(MaterialType::TEXTURE_DATA), pPixelData(pData), mExtent(extent), mPixelFormat(format), mType(type) {}

			/**
			 * Create the image using the loaded data.
			 *
			 * @param pDevice The device pointer.
			 * @param type The image type.
			 * @param usage The image usage.
			 * @param layers The image layers.
			 * @param mipLevels The number of mip levels to use. If set to 0, the best mip level is set.
			 * @param sampleCount The multi sample count. Default is One.
			 * @return The image pointer.
			 */
			std::unique_ptr<ImageT> CreateImage(DeviceT* pDevice, const ImageType type, const ImageUsage usage, const uint32_t layers, const uint32_t mipLevels, const MultiSampleCount sampleCount = MultiSampleCount::One);

			/**
			 * Clear texture data.
			 */
			void Clear();

		public:
			FBox3D mExtent = {};
			unsigned char* pPixelData = nullptr;
			PixelFormat mPixelFormat = PixelFormat::Undefined;
			TextureType mType = TextureType::Undefined;
		};
	}
}
