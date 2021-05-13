// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Buffers/ShaderResourceBuffer.h"

#include <filesystem>

namespace Flint
{
	namespace Backend
	{
		/**
		 * Shader location enum.
		 * This defines the location of the shader in the pipeline.
		 */
		enum class ShaderLocation : UI8 {
			UNDEFINED,
			VERTEX, TESSELLATION, GEOMETRY, FRAGMENT, COMPUTE,
		};

		/**
		 * Shader code type enum.
		 * This defines the type of the shader code.
		 */
		enum class ShaderCodeType : UI8 {
			UNDEFINED,
			GLSL, SPIR_V, HLSL
		};

		/**
		 * Uniform type enum.
		 * This defines what the uniform type is.
		 */
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
		 * Shader Attribute struct.
		 * This defines how a single Shader attribute is made up of.
		 */
		struct ShaderAttribute
		{
			String mName = {};
			UI64 mSize = 0;
			UI32 mLocation = 0;
			UI32 mLayerCount = 1;
		};

		/**
		 * Uniform Layout structure.
		 * This structure contains information about a single uniform object.
		 */
		struct UniformLayout
		{
			UniformLayout() = default;
			UniformLayout(UniformType type, UI32 binding) : mType(type), mBinding(binding) {}

			String mName = {};
			UI64 mSize = 0;
			UI32 mBinding = 0;
			UI32 mLayerCount = 1;
			UniformType mType = UniformType::UNDEFINED;
		};

		/**
		 * Flint Shader object.
		 */
		class Shader : public DeviceBoundObject
		{
		public:
			Shader(Device* pDevice, const std::filesystem::path& asset, const ShaderCodeType& codeType, const ShaderLocation& location);

			/**
			 * Get all the resource buffer names in the shader.
			 *
			 * @return The resource buffer names in the binding order.
			 */
			virtual std::vector<std::string> GetResourceBufferNames() const = 0;

			/**
			 * Bind resource buffers to the shader prior to being drawn.
			 *
			 * @param resourceName: The name of the buffer.
			 * @param pBuffer: The buffer to be binded.
			 */
			virtual void BindResourceBuffer(const std::string& resourceName, ShaderResourceBuffer* pBuffer) = 0;

		private:
			void PerformReflection();

		private:
			ShaderLocation mLocation = ShaderLocation::UNDEFINED;
			ShaderCodeType mCodeType = ShaderCodeType::UNDEFINED;

			std::vector<UI32> mShaderCode;
		};
	}
}