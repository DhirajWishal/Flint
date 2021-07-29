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
		VEC3 = 16,
		VEC4 = 16,
		UVEC1 = 4,
		UVEC2 = 8,
		UVEC3 = 16,
		UVEC4 = 16,
		SVEC1 = 4,
		SVEC2 = 8,
		SVEC3 = 16,
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
	 * Shader code type enum.
	 */
	enum class ShaderCodeType : UI8 {
		SPIR_V,
		GLSL,
		HLSL
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
	 * Shader code type enum.
	 */
	enum class ShaderType : UI8 {
		UNDEFINED,
		VERTEX, TESSELLATION_CONTROL, TESSELLATION_EVALUATION, GEOMETRY, FRAGMENT,
		COMPUTE,
		RAY_GEN, ANY_HIT, CLOSEST_HIT, RAY_MISS
	};

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

	protected:
		/**
		 * Terminate the device bound object.
		 */
		virtual void Terminate() = 0;

	protected:
		std::shared_ptr<Device> pDevice = nullptr;
	};
}