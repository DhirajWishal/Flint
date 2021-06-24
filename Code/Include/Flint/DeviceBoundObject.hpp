// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FObject.hpp"

#include <string>
#include <vector>
#include <filesystem>

namespace Flint
{
	class Device;

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
	 * Flint vertex descriptor.
	 * This describes the size and attributes of a single vertex.
	 */
	struct VertexDescriptor
	{
		VertexDescriptor() = default;
		VertexDescriptor(const std::vector<ShaderAttributeDataType>& attributes) : mAttributeTypes(attributes) {}

		/**
		 * Get the stride (size of) the vertex.
		 *
		 * @return The size of the vertex in bytes.
		 */
		UI64 Stride() const;

		std::vector<ShaderAttributeDataType> mAttributeTypes;
	};

	/**
	 * Flint device bound object.
	 * This object is the base class for all the resources in Flint which are based on a device.
	 */
	class DeviceBoundObject : public FObject
	{
		friend Device;

	public:
		DeviceBoundObject(Device& device) noexcept : mDevice(device) {}

	protected:
		/**
		 * Terminate the device bound object.
		 */
		virtual void Terminate() = 0;

	protected:
		Device& mDevice;
	};
}