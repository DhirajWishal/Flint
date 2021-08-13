// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FObject.hpp"
#include "Core/Error.hpp"

#include <vector>
#include <memory>

namespace Flint
{
	class Device;

	/**
	 * Shader attribute type enum.
	 * This defines what the shader attribute type is.
	 */
	enum class ShaderAttribueType : UI8 {
		UNDEFINED,
		POSITION, NORMAL, UV_COORDINATES,
		COLOR_0, COLOR_1, COLOR_2, COLOR_3,
		TEXTURE_COORDINATES_0, TEXTURE_COORDINATES_1, TEXTURE_COORDINATES_2, TEXTURE_COORDINATES_3,
		TEXTURE_COORDINATES_4, TEXTURE_COORDINATES_5, TEXTURE_COORDINATES_6, TEXTURE_COORDINATES_7,
		TANGENT, BITANGENT,
		BONE_ID, BONE_WEIGHT,
		CUSTOM
	};

	/**
	 * Vertex attribute data type enum.
	 * This enum contains all the supported vertex attribute data types.
	 */
	enum class ShaderAttributeDataType : UI8 {
		BYTE = 1,

		I8 = 1,
		UI8 = 1,
		SI8 = 1,
		I16 = 2,
		UI16 = 2,
		SI16 = 2,
		I32 = 4,
		UI32 = 4,
		SI32 = 4,
		I64 = 8,
		UI64 = 8,
		SI64 = 8,

		FLOAT = 4,
		F64 = 4,
		D64 = 8,
		CHAR = 1,

		PTR = 8,
		VOID_PTR = 8,

		VEC1 = 4,
		VEC2 = 8,
		VEC3 = 12,
		VEC4 = 16,
		UVEC1 = 4,
		UVEC2 = 8,
		UVEC3 = 12,
		UVEC4 = 16,
		SVEC1 = 4,
		SVEC2 = 8,
		SVEC3 = 12,
		SVEC4 = 16,
		BVEC1 = 1,
		BVEC2 = 2,
		BVEC3 = 3,
		BVEC4 = 4,

		MAT2 = 16,
		MAT3 = 36,
		MAT4 = 64,
	};

	/**
	 * Vertex attribute structure.
	 * This structure contains information about a single vertex attribute.
	 */
	struct VertexAttribute
	{
		VertexAttribute() = default;
		VertexAttribute(ShaderAttribueType type, ShaderAttributeDataType dataType) : mType(type), mDataType(dataType) {}

		ShaderAttribueType mType = ShaderAttribueType::UNDEFINED;
		ShaderAttributeDataType mDataType = ShaderAttributeDataType::VEC3;
	};

	/**
	 * Flint vertex descriptor.
	 * This describes the size and attributes of a single vertex.
	 */
	struct VertexDescriptor
	{
		VertexDescriptor() = default;
		VertexDescriptor(const std::vector<VertexAttribute>& attributes) : mAttributeTypes(attributes) {}

		/**
		 * Get the stride (size of) the vertex.
		 *
		 * @return The size of the vertex in bytes.
		 */
		UI64 Stride() const;

		std::vector<VertexAttribute> mAttributeTypes;
	};

	/**
	 * Buffer type enum.
	 */
	enum class BufferType : UI8 {
		UNDEFINED,
		STAGGING,
		VERTEX, INDEX,
		UNIFORM, STORAGE
	};

	/**
	 * Buffer memory profile enum.
	 * This enum defines the buffer's memory behavior.
	 *
	 * AUTOMATIC will set the default memory profile for a given buffer type.
	 * CPU_ONLY will set the content to the CPU bound memory (RAM).
	 * DEVICE_ONLY will set data in the device and stagging buffers are required to access them.
	 * TRANSFER_FRIENDLY will enable it to be mapped to the local address space.
	 */
	enum class BufferMemoryProfile : UI8 {
		AUTOMATIC,
		CPU_ONLY,
		DEVICE_ONLY,
		TRANSFER_FRIENDLY
	};

	/**
	 * Image type enum.
	 */
	enum class ImageType : UI8 {
		DIMENSIONS_1,
		DIMENSIONS_2,
		DIMENSIONS_3,
		CUBEMAP,

		DIMENSIONS_1_ARRAY,
		DIMENSIONS_2_ARRAY,
		DIMENSIONS_3_ARRAY,
		CUBEMAP_ARRAY,
	};

	/**
	 * Image usage enum.
	 */
	enum class ImageUsage : UI8 {
		GRAPHICS,
		STORAGE,
		DEPTH
	};

	/**
	 * Pixel format enum.
	 */
	enum class PixelFormat : UI8 {
		UNDEFINED,

		R8_SRGB,
		R8G8_SRGB,
		R8G8B8_SRGB,
		R8G8B8A8_SRGB,

		B8G8R8_SRGB,
		B8G8R8A8_SRGB,

		D16_SINT,
		D32_SFLOAT,
	};

	/**
	 * Address mode enum.
	 */
	enum class AddressMode : UI8 {
		REPEAT,
		MIRRORED_REPEAT,
		CLAMP_TO_EDGE,
		CLAMP_TO_BORDER,
		MIRROR_CLAMP_TO_EDGE,
	};

	/**
	 * Border color enum.
	 */
	enum class BorderColor : UI8 {
		FLOAT_TRANSPARENT_BLACK,
		INT_TRANSPARENT_BLACK,
		FLOAT_OPAQUE_BLACK,
		INT_OPAQUE_BLACK,
		FLOAT_OPAQUE_WHITE,
		INT_OPAQUE_WHITE,
	};

	/**
	 * Compare operator enum.
	 */
	enum class CompareOperator : UI8 {
		NEVER,
		LESS,
		EQUAL,
		LESS_OR_EQUAL,
		GREATER,
		NOT_EQUAL,
		GREATER_OR_EQUAL,
		ALWAYS,
	};

	/**
	 * Image filter enum.
	 */
	enum class ImageFilter : UI8 {
		NEAREST,
		LINEAR,
		CUBIC_IMG,
	};

	/**
	 *Image mip map mode enum.
	 */
	enum class ImageMipMapMode : UI8 {
		NEAREST,
		LINEAR,
	};

	/**
	 * Image sampler specification structure.
	 */
	struct ImageSamplerSpecification
	{
		// If set to 0.0f, the maximum supported will be set.
		float mMaxAnisotrophy = 0.0f;

		float mMaxLevelOfDetail = 0.0f;
		float mMinLevelOfDetail = 0.0f;

		float mMipLoadBias = 0.0f;

		AddressMode mAddressModeU = AddressMode::REPEAT;
		AddressMode mAddressModeV = AddressMode::REPEAT;
		AddressMode mAddressModeW = AddressMode::REPEAT;

		BorderColor mBorderColor = BorderColor::INT_OPAQUE_BLACK;

		CompareOperator mCompareOperator = CompareOperator::ALWAYS;

		ImageFilter mImageMagificationFilter = ImageFilter::LINEAR;
		ImageFilter mImageMinificationFilter = ImageFilter::LINEAR;

		ImageMipMapMode mMipMapMode = ImageMipMapMode::LINEAR;

		bool bEnableAnisotropy = true;
		bool bEnableCompare = false;
		bool bEnableUnnormalizedCoordinates = false;
	};

	/**
	 * Shader code type enum.
	 */
	enum class ShaderType : UI8 {
		UNDEFINED,
		VERTEX, TESSELLATION_CONTROL, TESSELLATION_EVALUATION, GEOMETRY, FRAGMENT,
		COMPUTE,
		RAY_GEN, ANY_HIT, CLOSEST_HIT, RAY_MISS
	};

	/**
	 * Off screen render target attachment enum.
	 */
	enum class OffScreenRenderTargetAttachment : UI8 {
		COLOR_BUFFER = BIT_SHIFT(0),
		DEPTH_BUFFER = BIT_SHIFT(1)
	};

	/**
	 * Bitwise OR operator for OffScreenRenderTargetAttachment.
	 *
	 * @param lhs: The left hand side argument.
	 * @param rhs: The right hand side argument.
	 * @return The OR-ed result.
	 */
	constexpr OffScreenRenderTargetAttachment operator|(const OffScreenRenderTargetAttachment& lhs, const OffScreenRenderTargetAttachment& rhs)
	{
		return static_cast<OffScreenRenderTargetAttachment>(static_cast<UI8>(lhs) | static_cast<UI8>(rhs));
	}

	/**
	 * Bitwise AND operator for OffScreenRenderTargetAttachment.
	 *
	 * @param lhs: The left hand side argument.
	 * @param rhs: The right hand side argument.
	 * @return The AND-ed result.
	 */
	constexpr OffScreenRenderTargetAttachment operator&(const OffScreenRenderTargetAttachment& lhs, const OffScreenRenderTargetAttachment& rhs)
	{
		return static_cast<OffScreenRenderTargetAttachment>(static_cast<UI8>(lhs) & static_cast<UI8>(rhs));
	}

	/**
	 * Rasterization samples enum.
	 */
	enum class RasterizationSamples : UI8 {
		BITS_1 = BIT_SHIFT(0),
		BITS_2 = BIT_SHIFT(1),
		BITS_4 = BIT_SHIFT(2),
		BITS_8 = BIT_SHIFT(3),
		BITS_16 = BIT_SHIFT(4),
		BITS_32 = BIT_SHIFT(5),
		BITS_64 = BIT_SHIFT(6),
	};

	/**
	 * Rasterization samples bitwise OR operator.
	 *
	 * @param lhs: The left hand side argument.
	 * @param rhs: The right hand side argument.
	 * @return The OR performed samples.
	 */
	constexpr RasterizationSamples operator|(const RasterizationSamples& lhs, const RasterizationSamples& rhs) { return static_cast<RasterizationSamples>(static_cast<UI8>(lhs) | static_cast<UI8>(rhs)); }

	/**
	 * Rasterization samples bitwise AND operator.
	 *
	 * @param lhs: The left hand side argument.
	 * @param rhs: The right hand side argument.
	 * @return The AND performed samples.
	 */
	constexpr RasterizationSamples operator&(const RasterizationSamples& lhs, const RasterizationSamples& rhs) { return static_cast<RasterizationSamples>(static_cast<UI8>(lhs) & static_cast<UI8>(rhs)); }

	/**
	 * Flint device bound object.
	 * This object is the base class for all the resources in Flint which are based on a device.
	 */
	class DeviceBoundObject : public FObject
	{
		friend Device;

	public:
		/**
		 * Default constructor.
		 *
		 * @param pDevice: The device pointer.
		 */
		DeviceBoundObject(const std::shared_ptr<Device>& pDevice);

		/**
		 * Get the device of this object.
		 *
		 * @return The device pointer.
		 */
		std::shared_ptr<Device> GetDevice() const { return pDevice; }

	protected:
		/**
		 * Terminate the device bound object.
		 */
		virtual void Terminate() = 0;

	protected:
		std::shared_ptr<Device> pDevice = nullptr;
	};
}