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
		String mName = {};
		UI64 mSize = 0;
		UI32 mLocation = 0;
		UI32 mLayerCount = 1;
	};

	/**
	 * Uniform Layout structure.
	 * This structure contains information about a single unifom object.
	 */
	struct UniformLayout {
		UniformLayout() = default;
		UniformLayout(UniformType type, UI32 binding) : mType(type), mBinding(binding) {}

		String mName = {};
		UI64 mSize = 0;
		UI32 mBinding = 0;
		UI32 mLayerCount = 1;
		UniformType mType = UniformType::UNDEFINED;
	};

	/**
	 * Shader Digest object.
	 * This contains the summary of the shader, including the shade code, inputs and output layouts, uniforms
	 * and other information.
	 */
	class ShaderDigest {
	public:
		ShaderDigest() = default;

		/**
		 * Construct the shader digest by initializing (copy)
		 *
		 * @param location: The location of the shader in the pipeline.
		 * @param uniformLayouts: The uniform layouts of the shader.
		 * @param code: The shader code.
		 */
		ShaderDigest(
			ShaderLocation location,
			const std::vector<UniformLayout>& uniformLayouts,
			const std::vector<UI32>& code)
			: mLocation(location), mUniforms(uniformLayouts), mCode(code) {}

		/**
		 * Construct the shader digest by initializing (move)
		 *
		 * @param location: The location of the shader in the pipeline.
		 * @param uniformLayouts: The uniform layouts of the shader.
		 * @param code: The shader code.
		 */
		ShaderDigest(
			ShaderLocation location,
			std::vector<UniformLayout>&& uniformLayouts,
			std::vector<UI32>&& code)
			: mLocation(std::move(location)), mUniforms(std::move(uniformLayouts)), mCode(std::move(code)) {}

		std::vector<ShaderAttribute> mInputAttributes;
		std::vector<ShaderAttribute> mOutputAttributes;
		std::vector<UniformLayout> mUniforms;
		std::vector<UI32> mCode;
		ShaderLocation mLocation = ShaderLocation::UNDEFINED;
	};
}