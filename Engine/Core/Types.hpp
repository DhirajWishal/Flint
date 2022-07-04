// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Containers/BinaryMap.hpp"

#include <cstdint>
#include <array>

#define FLINT_DEFINE_ENUM_AND_OR(name)																											\
	constexpr name operator|(const name& lhs, const name& rhs) { return static_cast<name>(::Flint::EnumToInt(lhs) | ::Flint::EnumToInt(rhs)); }	\
	constexpr name operator&(const name& lhs, const name& rhs) { return static_cast<name>(::Flint::EnumToInt(lhs) & ::Flint::EnumToInt(rhs)); }

#define FLINT_DEFINE_HANDLE(name)	enum class name : uintptr_t {}

namespace Flint
{
	/**
	 * Convert an enum to integer.
	 *
	 * @tparam Type The type of the enum.
	 * @param value The value to convert.
	 * @return The integer value it holds.
	 */
	template<class Type>
	[[nodiscard]] constexpr std::underlying_type_t<Type> EnumToInt(const Type value) noexcept
	{
		return static_cast<std::underlying_type_t<Type>>(value);
	}

	/**
	 * Convert an integer to an enum.
	 *
	 * @tparam Type The type of the data.
	 * @param value The value to convert.
	 * @return The converted enum.
	 */
	template<class Type>
	[[nodiscard]] constexpr Type IntToEnum(const std::underlying_type_t<Type> value) noexcept
	{
		return static_cast<Type>(value);
	}

	/**
	 * Pixel format enum.
	 */
	enum class PixelFormat : uint8_t
	{
		Undefined,

		R8_SRGB,
		R8G8_SRGB,
		R8G8B8_SRGB,
		R8G8B8A8_SRGB,

		R8_UNORMAL,
		R8G8_UNORMAL,
		R8G8B8_UNORMAL,
		R8G8B8A8_UNORMAL,

		B8G8R8_SRGB,
		B8G8R8A8_SRGB,

		B8G8R8_UNORMAL,
		B8G8R8A8_UNORMAL,

		R16_SFLOAT,
		R16G16_SFLOAT,
		R16G16B16_SFLOAT,
		R16G16B16A16_SFLOAT,

		R32_SFLOAT,
		R32G32_SFLOAT,
		R32G32B32_SFLOAT,
		R32G32B32A32_SFLOAT,

		D16_SINT,
		D32_SFLOAT,

		S8_UINT,
		D16_UNORMAL_S8_UINT,
		D24_UNORMAL_S8_UINT,
		D32_SFLOAT_S8_UINT,
	};

	/**
	 * Get the pixel format's size.
	 *
	 * @param format The pixel format.
	 * @return The size in bytes.
	 */
	constexpr uint8_t GetPixelSize(PixelFormat format)
	{
		switch (format)
		{
		case Flint::PixelFormat::R8_SRGB:
		case Flint::PixelFormat::R8_UNORMAL:
		case Flint::PixelFormat::S8_UINT:
			return 1;

		case Flint::PixelFormat::R8G8_SRGB:
		case Flint::PixelFormat::R8G8_UNORMAL:
		case Flint::PixelFormat::R16_SFLOAT:
		case Flint::PixelFormat::D16_SINT:
			return 2;

		case Flint::PixelFormat::R8G8B8_SRGB:
		case Flint::PixelFormat::R8G8B8_UNORMAL:
		case Flint::PixelFormat::B8G8R8_SRGB:
		case Flint::PixelFormat::B8G8R8_UNORMAL:
		case Flint::PixelFormat::D16_UNORMAL_S8_UINT:
			return 3;

		case Flint::PixelFormat::R8G8B8A8_SRGB:
		case Flint::PixelFormat::R8G8B8A8_UNORMAL:
		case Flint::PixelFormat::B8G8R8A8_SRGB:
		case Flint::PixelFormat::B8G8R8A8_UNORMAL:
		case Flint::PixelFormat::R16G16_SFLOAT:
		case Flint::PixelFormat::R32_SFLOAT:
		case Flint::PixelFormat::D32_SFLOAT:
		case Flint::PixelFormat::D24_UNORMAL_S8_UINT:
			return 4;

		case Flint::PixelFormat::D32_SFLOAT_S8_UINT:
			return 5;

		case Flint::PixelFormat::R16G16B16_SFLOAT:
			return 6;

		case Flint::PixelFormat::R16G16B16A16_SFLOAT:
		case Flint::PixelFormat::R32G32_SFLOAT:
			return 8;

		case Flint::PixelFormat::R32G32B32_SFLOAT:
			return 12;

		case Flint::PixelFormat::R32G32B32A32_SFLOAT:
			return 16;

		default:
			return 0;
		}
	}

	/**
	 * Attachment type enum.
	 */
	enum class AttachmentType : uint8_t
	{
		Color,	// This is used for rasterizing.
		Depth,	// This is used for rasterizing.
		Storage	// This is used for ray tracing.
	};

	/**
	 * Attachment description structure.
	 * This structure describes information about a single render target attachment.
	 */
	struct AttachmentDescription final
	{
		PixelFormat m_Format = PixelFormat::Undefined;	// If set to undefined, the best suitable will be set automatically.
		AttachmentType m_Type = AttachmentType::Color;
	};

	namespace Defaults
	{
		/**
		 * Color attachment description variable.
		 * This can be used to specify a default color attachment.
		 */
		constexpr auto ColorAttachmentDescription = AttachmentDescription{ PixelFormat::Undefined, AttachmentType::Color };

		/**
		 * Depth attachment description variable.
		 * This can be used to specify a default depth attachment.
		 */
		constexpr auto DepthAttachmentDescription = AttachmentDescription{ PixelFormat::Undefined, AttachmentType::Depth };
	}

	/**
	 * Data type enum.
	 */
	enum class DataType : uint8_t
	{
		None = 0,								// Default type.

		Float,		// 32 bit float type.

		Vec2_8,		// Two component, 8 bit vector.
		Vec2_16,	// Two component, 16 bit vector.
		Vec2_32,	// Two component, 32 bit vector.
		Vec2_64,	// Two component, 64 bit vector.

		Vec3_8,		// Three component, 8 bit vector.
		Vec3_16,	// Three component, 16 bit vector.
		Vec3_32,	// Three component, 32 bit vector.
		Vec3_64,	// Three component, 64 bit vector.

		Vec4_8,		// Four component, 8 bit vector.
		Vec4_16,	// Four component, 16 bit vector.
		Vec4_32,	// Four component, 32 bit vector.
		Vec4_64,	// Four component, 64 bit vector.

		Max			// Maximum size of the enum.
	};

	/**
	 * Data type size array.
	 * This contains all the sizes of the data types in the DataType enum.
	 */
	constexpr std::array<uint8_t, EnumToInt(DataType::Max)> DataTypeSize =
	{
		0,

		sizeof(float),

		sizeof(uint8_t[2]),
		sizeof(uint16_t[2]),
		sizeof(uint32_t[2]),
		sizeof(uint64_t[2]),

		sizeof(uint8_t[3]),
		sizeof(uint16_t[3]),
		sizeof(uint32_t[3]),
		sizeof(uint64_t[3]),

		sizeof(uint8_t[4]),
		sizeof(uint16_t[4]),
		sizeof(uint32_t[4]),
		sizeof(uint64_t[4]),
	};

	/**
	 * Vertex attribute enum.
	 */
	enum class VertexAttribute : uint8_t
	{
		Position,
		Normal,

		Tangent,
		BiTangent,

		Color0,
		Color1,
		Color2,
		Color3,
		Color4,
		Color5,
		Color6,
		Color7,

		Texture0,
		Texture1,
		Texture2,
		Texture3,
		Texture4,
		Texture5,
		Texture6,
		Texture7,

		Max
	};

	/**
	 * Vertex descriptor type.
	 * This defines all the types in a vertex.
	 */
	using VertexDescriptor = std::array<DataType, EnumToInt(VertexAttribute::Max)>;

	/**
	 * Vertex attribute bits type.
	 * This is used primarily to quickly check if a vertex attribute is available.
	 */
	using VertexAttributeBits = uint32_t;

	/**
	 * Get the vertex attribute's bit value.
	 *
	 * @param attribute The vertex attribute to get the bit value of.
	 * @return The bit value.
	 */
	constexpr VertexAttributeBits GetVertexAttributeBit(const VertexAttribute attribute) { return 1 << EnumToInt(attribute); }

	/**
	 * Vertex input structure.
	 * This contains information about a single vertex input.
	 */
	struct VertexInput final
	{
		uint8_t m_Components = 1;	// This specifies the number of components in the type. For example, vec3 = 3, vec2 = 2, float = 1.
		VertexAttribute m_Attribute = VertexAttribute::Max;
	};

	/**
	 * Instance attribute enum.
	 */
	enum class InstanceAttribute : uint8_t
	{
		InstanceID,
		Position,
		Rotation,
		Scale,

		Max
	};

	/**
	 * Instance descriptor type.
	 * This defines all the types in an instance.
	 */
	using InstanceDescriptor = std::array<DataType, EnumToInt(InstanceAttribute::Max)>;

	/**
	 * Instance input structure.
	 * This structure contains information about a single instance input attribute.
	 */
	struct InstanceInput final
	{
		uint8_t m_Components = 1;	// This specifies the number of components in the type. For example, vec3 = 3, vec2 = 2, float = 1.
		InstanceAttribute m_Attribute = InstanceAttribute::Max;
	};

	/**
	 * Get the stride from the vertex or instance descriptor.
	 *
	 * @tparam Size The size of the array.
	 * @param descriptor The descriptor.
	 * @return The stride (the descriptor size).
	 */
	template<uint64_t Size>
	constexpr uint64_t GetStride(const std::array<DataType, Size>& descriptor) noexcept
	{
		uint64_t stride = 0;
		for (const auto type : descriptor)
			stride += DataTypeSize[EnumToInt(type)];

		return stride;
	}

	/**
	 * Buffer usage enum.
	 */
	enum class BufferUsage : uint8_t
	{
		// Used to store vertex data. Note that in order to supply data to this type, we need a staging buffer.
		Vertex = 1 << 0,

		// Used to store index data. Note that in order to supply data to this type, we need a staging buffer.
		Index = 1 << 1,

		// Used to store vertex data. Note that unlike the other, this can directly receive data.
		ShallowVertex = 1 << 2,

		// Used to store index data. Note that unlike the other, this can directly receive data.
		ShallowIndex = 1 << 3,

		// Used to store uniform data.
		Uniform = 1 << 4,

		// Used to store data from the shader.
		Storage = 1 << 5,

		// Used for both uniform and storage purposes.
		General = Uniform | Storage,

		// Used for data transferring purposes.
		Staging = 1 << 6
	};

	/**
	 * Image usage enum.
	 */
	enum class ImageUsage : uint8_t
	{
		Graphics = 1 << 0,
		Storage = 1 << 1,
	};

	FLINT_DEFINE_ENUM_AND_OR(ImageUsage);

	/**
	 * Shader resource type enum.
	 */
	enum class ResourceType : uint8_t
	{
		Undefined,

		Sampler,
		CombinedImageSampler,
		SampledImage,
		StorageImage,
		UniformTexelBuffer,
		StorageTexelBuffer,
		UniformBuffer,
		StorageBuffer,
		DynamicUniformBuffer,
		DynamicStorageBuffer,
		InputAttachment,
		AccelerationStructure
	};

	/**
	 * Multisample enum.
	 */
	enum class Multisample : uint8_t
	{
		One = 1 << 0,
		Two = 1 << 1,
		Four = 1 << 2,
		Eight = 1 << 3,
		Sixteen = 1 << 4,
		ThirtyTwo = 1 << 5,
		SixtyFour = 1 << 6
	};

	FLINT_DEFINE_ENUM_AND_OR(Multisample);

	/**
	 * Address mode enum.
	 */
	enum class AddressMode : uint8_t
	{
		Repeat,
		MirroredRepeat,
		ClampToEdge,
		ClampToBorder,
		MirrorClampToEdge,
	};

	/**
	 * Border color enum.
	 */
	enum class BorderColor : uint8_t
	{
		TransparentBlackFLOAT,
		TransparentBlackINT,
		OpaqueBlackFLOAT,
		OpaqueBlackINT,
		OpaqueWhiteFLOAT,
		OpaqueWhiteINT,
	};

	/**
	 * Compare operator enum.
	 */
	enum class CompareOperator : uint8_t
	{
		Never,
		Less,
		Equal,
		LessOrEqual,
		Greater,
		NotEqual,
		GreaterOrEqual,
		Always,
	};

	/**
	 * Image filter enum.
	 */
	enum class ImageFilter : uint8_t
	{
		Nearest,
		Linear,
		CubicImage,
	};

	/**
	 *Image mip map mode enum.
	 */
	enum class ImageMipMapMode : uint8_t
	{
		Nearest,
		Linear,
	};

	/**
	 * Texture sampler specification structure.
	 */
	struct TextureSamplerSpecification
	{
		// If set to 0.0f, the maximum supported will be set.
		float mMaxAnisotrophy = 0.0f;

		float mMaxLevelOfDetail = 0.0f;
		float mMinLevelOfDetail = 0.0f;

		float mMipLODBias = 0.0f;

		AddressMode mAddressModeU = AddressMode::Repeat;
		AddressMode mAddressModeV = AddressMode::Repeat;
		AddressMode mAddressModeW = AddressMode::Repeat;

		BorderColor mBorderColor = BorderColor::OpaqueWhiteFLOAT;

		CompareOperator mCompareOperator = CompareOperator::Always;

		ImageFilter mImageMagificationFilter = ImageFilter::Linear;
		ImageFilter mImageMinificationFilter = ImageFilter::Linear;

		ImageMipMapMode mMipMapMode = ImageMipMapMode::Linear;

		bool bEnableAnisotropy = true;
		bool bEnableCompare = false;
		bool bEnableUnnormalizedCoordinates = false;
	};
}