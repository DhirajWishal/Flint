// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Shader resource type enum.
		 */
		enum class ShaderResourceType : UI8 {
			UNIFORM_BUFFER,
			STORAGE_BUFFER,
			SAMPLER
		};

		/**
		 * Shader resource structure.
		 */
		struct ShaderResource
		{
			ShaderResource() = default;
			ShaderResource(const std::string& name, UI32 binding, ShaderResourceType type) : mResourceName(name), mBinding(binding), mType(type) {}

			std::string mResourceName = "";
			UI32 mBinding = 0;
			ShaderResourceType mType = ShaderResourceType::UNIFORM_BUFFER;
		};

		/**
		 * Shader attribute structure.
		 */
		struct ShaderAttribute
		{
			ShaderAttribute() = default;
			ShaderAttribute(const std::string& name, UI32 location, ShaderAttributeDataType type) : mAttributeName(name), mLocation(location), mDataType(type) {}

			std::string mAttributeName = "";
			UI32 mLocation = 0;
			ShaderAttributeDataType mDataType = ShaderAttributeDataType::VEC3;
		};

		/**
		 * Flint shader object.
		 * This object is the base class for all the shader objects.
		 * Shader is a program which is run on the device and performs different tasks depending on its stage/ location.
		 *
		 * Internally, we use SPIRV to store shaders.
		 */
		class Shader : public DeviceBoundObject
		{
		public:
			Shader(Device& device, const std::filesystem::path& path, ShaderCodeType type) : DeviceBoundObject(device) {}
			Shader(Device& device, const std::vector<UI32>& code, ShaderCodeType type = ShaderCodeType::SPIR_V) : DeviceBoundObject(device) {}
			Shader(Device& device, const std::string& code, ShaderCodeType type = ShaderCodeType::GLSL) : DeviceBoundObject(device) {}

			/**
			 * Get the shader resources of the shader.
			 *
			 * @return The shader resources.
			 */
			std::vector<ShaderResource> GetShaderResources() const { return mResources; }

			/**
			 * Get shader input attributes.
			 * Data to the shaders are passed via input attributes.
			 *
			 * @return The array of input attributes.
			 */
			std::vector<ShaderAttribute> GetInputAttributes() const { return mInputAttributes; }

			/**
			 * Get shader output attributes.
			 * Data from one shader to another shader are passed using output attributes.
			 *
			 * @return The array of output attributes.
			 */
			std::vector<ShaderAttribute> GetOutputAttributes() const { return mOutputAttributes; }

		protected:
			std::vector<ShaderResource> mResources;
			std::vector<ShaderAttribute> mInputAttributes;
			std::vector<ShaderAttribute> mOutputAttributes;
		};
	}
}