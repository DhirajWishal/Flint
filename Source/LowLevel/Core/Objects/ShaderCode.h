// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Types/DataTypes.h"

namespace Flint
{
	enum class ShaderLocation : UI8 {
		UNDEFINED,
		VERTEX, TESSELLATION, GEOMETRY, FRAGMENT, COMPUTE,
	};

	enum class ShaderType : UI8 {
		UNDEFINED,
		GLSL, SPIR_V, HLSL
	};

	enum class UniformType : UI8 {
		UNDEFINED,
		UNIFORM_BUFFER, STORAGE_BUFFER, UNIFORM_BUFFER_DYNAMIC, STORAGE_BUFFER_DYNAMIC,
		UNIFORM_TEXEL_BUFFER, STORAGE_TEXEL_BUFFER,
		INPUT_ATTACHMENT,
		STORAGE_IMAGE,
		CONSTANT,
		SAMPLER_2D, SAMPLER_CUBE, SAMPLER_2D_ARRAY, SAMPLER_CUBE_ARRAY,
		ACCELERATION_STRUCTURE,
	};

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
	 * Shader Attribute struct.
	 * This defines how a single Shader attribute is made up of.
	 */
	struct ShaderAttribute {
		UI64 mSize = 0;
		ShaderAttribueType mType = ShaderAttribueType::UNDEFINED;

		/**
		 * Get the size of the attribute.
		 *
		 * @return The size in bytes.
		 */
		UI64 Size() const { return mSize; }
	};

	/**
	 * Shader Code object.
	 * This objest stores shader code which will then be converted to shader modules by the backends.
	 */
	class ShaderCode {
		struct Uniform {
			UI64 mSize = 0;
			UniformType mType = UniformType::UNDEFINED;
		};

		struct IOLayout {
			std::vector<ShaderAttribute> mAttributes;
			UI8 mLocation = 0;

			/**
			 * Add an attribue to the layout.
			 *
			 * @param attribtue: The attribute to be added.
			 */
			void AddAttribute(const ShaderAttribute& attribute) { INSERT_INTO_VECTOR(mAttributes, attribute); }

			/**
			 * Get the stride of the layout.
			 * Stride is the number of bytes to move up to find the next vertex in the vertex buffer.
			 *
			 * @return The stride in bytes.
			 */
			UI64 Stride() const
			{
				UI64 size = 0;
				for (auto itr = mAttributes.begin(); itr != mAttributes.end(); itr++)
					size += itr->Size();

				return size;
			}
		};

	public:
		ShaderCode() {}
		~ShaderCode() {}

		/**
		 * Initialize the shader code by loading it to memory.
		 *
		 * @param pAsset: The asset path.
		 * @param location: The location of the shader in the pipeline.
		 * @param type: The shader code type.
		 */
		void Initialize(const char* pAsset, ShaderLocation location, ShaderType type);

		/**
		 * Perform reflection on the shader code.
		 * The code must be loaded beforehand.
		 */
		void Reflect();

		/**
		 * Terminate the shader code by dumping the code.
		 */
		void Terminate();

	private:
		std::vector<UI32> mCode;
		ShaderType mType = ShaderType::UNDEFINED;
		ShaderLocation mLocation = ShaderLocation::UNDEFINED;
	};
}