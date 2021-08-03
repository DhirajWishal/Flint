// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

#include <unordered_map>

namespace Flint
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
		ShaderResource(UI32 binding, UI32 setIndex, ShaderResourceType type) : mBinding(binding), mSetIndex(setIndex), mType(type) {}

		UI32 mBinding = 0;
		UI32 mSetIndex = 0;
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
		/**
		 * Construct the shader using a shader asset file.
		 * This asset file is recommended to be a SPIRV binary.
		 *
		 * @param pDevice: The device pointer.
		 * @param type: The shader type.
		 * @param path: The file path to the asset file.
		 * @param codeType: The shader code type.
		 */
		Shader(const std::shared_ptr<Device>& pDevice, ShaderType type, const std::filesystem::path& path, ShaderCodeType codeType);

		/**
		 * Construct the shader using a shader code.
		 * This code can be of any type.
		 *
		 * @param pDevice: The device pointer.
		 * @param type: The shader type.
		 * @param code: The shader code as a vector of UI32.
		 * @param codeType: The shader code type.
		 */
		Shader(const std::shared_ptr<Device>& pDevice, ShaderType type, const std::vector<UI32>& code, ShaderCodeType codeType = ShaderCodeType::SPIR_V);

		/**
		 * Construct the shader using a shader code.
		 * This code can be of any type.
		 *
		 * @param pDevice: The device pointer.
		 * @param type: The shader type.
		 * @param code: The shader code as a string.
		 * @param codeType: The shader code type.
		 */
		Shader(const std::shared_ptr<Device>& pDevice, ShaderType type, const std::string& code, ShaderCodeType codeType = ShaderCodeType::GLSL);

		/**
		 * Get the shader resources of the shader.
		 *
		 * @return The shader resources.
		 */
		const std::unordered_map<std::string, ShaderResource> GetShaderResources() const { return mResources; }

		/**
		 * Get shader input attributes.
		 * Data to the shaders are passed via input attributes.
		 *
		 * @return The array of input attributes.
		 */
		const std::unordered_map<UI32, std::vector<ShaderAttribute>> GetInputAttributes() const { return mInputAttributes; }

		/**
		 * Get shader output attributes.
		 * Data from one shader to another shader are passed using output attributes.
		 *
		 * @return The array of output attributes.
		 */
		const std::unordered_map<UI32, std::vector<ShaderAttribute>> GetOutputAttributes() const { return mOutputAttributes; }

	protected:
		std::unordered_map<std::string, ShaderResource> mResources;
		std::unordered_map<UI32, std::vector<ShaderAttribute>> mInputAttributes;
		std::unordered_map<UI32, std::vector<ShaderAttribute>> mOutputAttributes;

		ShaderType mType = ShaderType::UNDEFINED;
	};
}