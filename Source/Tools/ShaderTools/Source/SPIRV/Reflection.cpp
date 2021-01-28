// Copyright 2020 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ShaderTools/SPIRV/Reflection.h"
#include "Core/ErrorHandler/Logger.h"
#include "ShaderTools/SPIRV/Utilities.h"

#include <spirv_glsl.hpp>

namespace Flint
{
	namespace ShaderTools
	{
		ShaderDigest SPIRVReflection::Reflect(const std::vector<UI32>& mShaderCode, ShaderLocation location)
		{
			// Create the reflection digest object.
			ShaderDigest mDigest = {};
			mDigest.mCode = mShaderCode;
			mDigest.mLocation = location;

			// Setup the compiler and data structures.
			spirv_cross::Compiler mCompiler(std::move(Utilities::ResolvePadding(mShaderCode)));
			spirv_cross::ShaderResources mResources = mCompiler.get_shader_resources();
			spirv_cross::SPIRType mType = {};

			// Calculate the shader offset.
			UI64 mShaderOffset = 0;

			// Resolve uniform buffers.
			for (auto& resource : mResources.uniform_buffers)
			{
				// Create the uniform buffer object.
				UniformLayout mUniform(UniformType::UNIFORM_BUFFER, mCompiler.get_decoration(resource.id, spv::DecorationBinding));
				mUniform.mLayerCount = mCompiler.get_type(resource.base_type_id).vecsize;
				mUniform.mName = std::move(mCompiler.get_name(resource.id));

				UI32 byteSize = 0;
				// Resolve the uniform attributes.
				for (UI32 index = 0; index < mCompiler.get_type(resource.base_type_id).member_types.size(); index++)
				{
					auto& Ty = mCompiler.get_type(mCompiler.get_type(resource.base_type_id).member_types[index]);
					byteSize += (Ty.width / 8) * Ty.vecsize * Ty.columns;
				}

				mUniform.mSize = byteSize;

				INSERT_INTO_VECTOR(mDigest.mUniforms, std::move(mUniform));
				mShaderOffset += byteSize;
			}

			// Resolve storage buffers.
			for (auto& resource : mResources.storage_buffers)
			{
				// Create the storage buffer object.
				UniformLayout mUniform(UniformType::STORAGE_BUFFER, mCompiler.get_decoration(resource.id, spv::DecorationBinding));
				mUniform.mLayerCount = mCompiler.get_type(resource.base_type_id).vecsize;
				mUniform.mName = std::move(mCompiler.get_name(resource.id));

				UI32 byteSize = 0;
				// Resolve the uniform attributes.
				for (UI32 index = 0; index < mCompiler.get_type(resource.base_type_id).member_types.size(); index++)
				{
					auto& Ty = mCompiler.get_type(mCompiler.get_type(resource.base_type_id).member_types[index]);
					byteSize += (Ty.width / 8) * Ty.vecsize * Ty.columns;
				}

				mUniform.mSize = byteSize;
				INSERT_INTO_VECTOR(mDigest.mUniforms, std::move(mUniform));
				mShaderOffset += byteSize;
			}

			//// Resolve shader inputs.
			//ShaderIOLayout mInputAttribute = {};
			//for (auto& resource : mResources.stage_inputs)
			//{
			//	// Create the input attribute.
			//	mInputAttribute.mLocation = mCompiler.get_decoration(resource.id, spv::DecorationLocation);
			//	//mInputAttribute.mBinding = mCompiler.get_decoration(resource.id, spv::DecorationBinding);
			//	mInputAttribute.mName = std::move(mCompiler.get_name(resource.id));
			//
			//	auto& Ty = mCompiler.get_type(resource.base_type_id);
			//	mInputAttribute.mLayerCount = Ty.columns;
			//	mInputAttribute.mDataType = static_cast<DataType>((Ty.width / 8) * (Ty.vecsize == 3 ? 4 : Ty.vecsize));
			//
			//	mDigest.mInputAttributes.insert(mDigest.mInputAttributes.end(), std::move(mInputAttribute));
			//
			//	mInputAttribute.mOffset += mInputAttribute.mLayerCount * (Ty.width / 8) * (Ty.vecsize == 3 ? 4 : Ty.vecsize);
			//}
			//
			//
			//// Resolve shader outputs.
			//ShaderIOLayout mOutputAttribute = {};
			//for (auto& resource : mResources.stage_outputs)
			//{
			//	// Create the output attribute.
			//	mOutputAttribute.mLocation = mCompiler.get_decoration(resource.id, spv::DecorationLocation);
			//	//mOutputAttribute.mBinding = mCompiler.get_decoration(resource.id, spv::DecorationBinding);
			//	mOutputAttribute.mName = std::move(mCompiler.get_name(resource.id));
			//
			//	auto& Ty = mCompiler.get_type(resource.base_type_id);
			//	mOutputAttribute.mLayerCount = Ty.columns;
			//	mOutputAttribute.mDataType = static_cast<DataType>((Ty.width / 8) * (Ty.vecsize == 3 ? 4 : Ty.vecsize));
			//
			//	mDigest.mOutputAttributes.insert(mDigest.mOutputAttributes.end(), std::move(mOutputAttribute));
			//
			//	mOutputAttribute.mOffset += mOutputAttribute.mLayerCount * (Ty.width / 8) * (Ty.vecsize == 3 ? 4 : Ty.vecsize);
			//}

			// Resolve storage images.
			for (auto& resource : mResources.storage_images)
			{
				// Create the storage image uniform object.
				UniformLayout mUniform(UniformType::STORAGE_IMAGE, mCompiler.get_decoration(resource.id, spv::DecorationBinding));
				mUniform.mLayerCount = mCompiler.get_type(resource.base_type_id).vecsize;
				mUniform.mName = std::move(mCompiler.get_name(resource.id));
				INSERT_INTO_VECTOR(mDigest.mUniforms, std::move(mUniform));
			}

			// Resolve image sampler.
			for (auto& resource : mResources.sampled_images)
			{
				// Create the image sampler uniform object.
				UniformLayout mUniform(UniformType::SAMPLER_2D, mCompiler.get_decoration(resource.id, spv::DecorationBinding));
				mUniform.mLayerCount = mCompiler.get_type(resource.base_type_id).vecsize;
				mUniform.mName = std::move(mCompiler.get_name(resource.id));
				INSERT_INTO_VECTOR(mDigest.mUniforms, std::move(mUniform));
			}

			// Resolve seperate images.
			for (auto& resource : mResources.separate_images)
			{
				// Create the seperate image uniform object.
				UniformLayout mUniform(UniformType::SAMPLER_2D, mCompiler.get_decoration(resource.id, spv::DecorationBinding));
				mUniform.mLayerCount = mCompiler.get_type(resource.base_type_id).vecsize;
				mUniform.mName = std::move(mCompiler.get_name(resource.id));
				INSERT_INTO_VECTOR(mDigest.mUniforms, std::move(mUniform));
			}

			// Resolve seperate sampler.
			for (auto& resource : mResources.separate_samplers)
			{
				// Create the seperate sampler uniform object.
				UniformLayout mUniform(UniformType::SAMPLER_2D, mCompiler.get_decoration(resource.id, spv::DecorationBinding));
				mUniform.mLayerCount = mCompiler.get_type(resource.base_type_id).vecsize;
				mUniform.mName = std::move(mCompiler.get_name(resource.id));
				INSERT_INTO_VECTOR(mDigest.mUniforms, std::move(mUniform));
			}

			// Resolve accelleration structure.
			for (auto& resource : mResources.acceleration_structures)
			{
				// Create the acceleration structure uniform.
				UniformLayout mUniform(UniformType::ACCELERATION_STRUCTURE, mCompiler.get_decoration(resource.id, spv::DecorationBinding));
				mUniform.mLayerCount = mCompiler.get_type(resource.base_type_id).vecsize;
				mUniform.mName = std::move(mCompiler.get_name(resource.id));

				UI32 byteSize = 0;
				// Resolve the structure attributes.
				for (UI32 index = 0; index < mCompiler.get_type(resource.base_type_id).member_types.size(); index++)
				{
					auto& Ty = mCompiler.get_type(mCompiler.get_type(resource.base_type_id).member_types[index]);
					byteSize += (Ty.width / 8) * Ty.vecsize * Ty.columns;
				}

				mUniform.mSize = byteSize;
				INSERT_INTO_VECTOR(mDigest.mUniforms, std::move(mUniform));
				mShaderOffset += byteSize;
			}

			// Resolve constant buffers.
			for (auto& resource : mResources.push_constant_buffers)
			{
				// Create the push constant object.
				UniformLayout mUniform(UniformType::CONSTANT, mCompiler.get_decoration(resource.id, spv::DecorationBinding));
				mUniform.mLayerCount = mCompiler.get_type(resource.base_type_id).vecsize;
				mUniform.mName = std::move(mCompiler.get_name(resource.id));

				UI32 byteSize = 0;
				// Resolve the buffer attributes.
				for (UI32 index = 0; index < mCompiler.get_type(resource.base_type_id).member_types.size(); index++)
				{
					auto& Ty = mCompiler.get_type(mCompiler.get_type(resource.base_type_id).member_types[index]);
					byteSize += (Ty.width / 8) * Ty.vecsize * Ty.columns;
				}

				mUniform.mSize = byteSize;
				INSERT_INTO_VECTOR(mDigest.mUniforms, std::move(mUniform));
				mShaderOffset += byteSize;
			}

			return mDigest;
		}
	}
}