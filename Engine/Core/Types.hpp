// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Containers/BinaryMap.hpp"

#include <cstdint>
#include <array>

#define FLINT_DEFINE_ENUM_AND_OR(name)																											\
	constexpr name operator|(const name& lhs, const name& rhs) { return static_cast<name>(::Flint::Core::EnumToInt(lhs) | ::Flint::Core::EnumToInt(rhs)); }	\
	constexpr name operator&(const name& lhs, const name& rhs) { return static_cast<name>(::Flint::Core::EnumToInt(lhs) & ::Flint::Core::EnumToInt(rhs)); }

#define FLINT_DEFINE_HANDLE(name)	enum class name : uintptr_t {}

namespace Flint
{
	namespace Core
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
		 * Attachment type enum.
		 */
		enum class AttachmentType : uint8_t
		{
			Color,
			Depth
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
		 * Instance attribute enum.
		 */
		enum class InstanceAttribute : uint8_t
		{
			Position,
			Rotation,
			Scale,
			GeometryID,

			Max
		};

		/**
		 * Instance descriptor type.
		 * This defines all the types in an instance.
		 */
		using InstanceDescriptor = std::array<DataType, EnumToInt(InstanceAttribute::Max)>;

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
		 * Image usage enum.
		 */
		enum class ImageUsage : uint8_t
		{
			Graphics = 1 << 0,
			Storage = 1 << 1,
		};

		FLINT_DEFINE_ENUM_AND_OR(ImageUsage);

		/**
		 * Image handle enum.
		 * This is used to uniquely identify an image which is bound to a device.
		 */
		FLINT_DEFINE_HANDLE(ImageHandle);

		/**
		 * Image view handle enum.
		 */
		FLINT_DEFINE_HANDLE(ImageViewHandle);

		/**
		 * Image sampler handle enum.
		 */
		FLINT_DEFINE_HANDLE(ImageSamplerHandle);

		enum class ImageViewType : uint8_t
		{
		};

		/**
		 * Image binding specification.
		 * This describes how the image should be bound and rendered.
		 */
		struct ImageBindingSpecification final
		{

		};

		/**
		 * Resource binding table structure.
		 * This defines which resource is bound to which shader resource, and is used to submit the data to the shaders when drawing.
		 */
		struct ResourceBindingTable final
		{
			/**
			 * Bind an image to the table.
			 * If multiple handles are bound to the same binding, it is considered to be an array.
			 *
			 * @param binding The shader binding to which the image is bound to.
			 * @param handle The image handle.
			 */
			void bind(uint32_t binding, ImageHandle handle) { m_Images[binding].emplace_back(handle); }

			/**
			 * Bind a buffer to the table.
			 * If multiple handles are bound to the same binding, it is considered to be an array.
			 *
			 * @param binding The shader binding to which the buffer is bound to.
			 * @param handle The buffer handle.
			 */
			//void bind(uint32_t binding, BufferHandle handle) { m_Buffers[binding].emplace_back(handle); }

		public:
			BinaryMap<uint32_t, std::vector<ImageHandle>> m_Images;
			//BinaryMap<uint32_t, std::vector<BufferHandle>> m_Buffers;
		};

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
	}
}