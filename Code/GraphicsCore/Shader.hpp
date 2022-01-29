// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DeviceBoundObject.hpp"

namespace std
{
	/**
	 * STL hash specialization.
	 */
	template<>
	struct hash<Flint::ShaderResourceKey>
	{
		constexpr size_t operator()(const Flint::ShaderResourceKey& resourceKey) const
		{
			return static_cast<size_t>(resourceKey.mSetIndex) << 32 | static_cast<size_t>(resourceKey.mBindingIndex);
		}
	};
}

namespace Flint
{
	/**
	 * Flint shader object.
	 * This object is the base class for all the shader objects.
	 * Shader is a program which is run on the device and performs different tasks depending on its stage/ location.
	 *
	 * We require shaders to be in the SPIR-V format.
	 */
	class Shader : public DeviceBoundObject
	{
	public:
		/**
		 * Construct the shader using a shader asset file.
		 * This asset file is recommended to be a SPIRV binary.
		 *
		 * @param pDevice The device pointer.
		 * @param type The shader type.
		 * @param path The file path to the asset file.
		 */
		Shader(const std::shared_ptr<Device>& pDevice, const ShaderType type, const std::filesystem::path& path);

		/**
		 * Construct the shader using a shader code.
		 * This code can be of any type.
		 *
		 * @param pDevice The device pointer.
		 * @param type The shader type.
		 * @param code The shader code as a vector of uint32.
		 */
		Shader(const std::shared_ptr<Device>& pDevice, const ShaderType type, const std::vector<uint32>& code);

		/**
		 * Construct the shader using a shader code.
		 * This code can be of any type.
		 *
		 * @param pDevice The device pointer.
		 * @param type The shader type.
		 * @param code The shader code as a string.
		 */
		Shader(const std::shared_ptr<Device>& pDevice, const ShaderType type, const std::string& code);

		/**
		 * Create the shader code cache.
		 * Code cache is easy to load and is much more efficient.
		 *
		 * @param path The path to save the shader code. Make sure to provide the file name!
		 */
		virtual void CreateCache(const std::filesystem::path& path) const = 0;

		/**
		 * Reload the shader using the shader file.
		 *
		 * @param path The shader file path.
		 */
		virtual void Reload(const std::filesystem::path& path) = 0;

		/**
		 * Reload the shader using the shader code vector.
		 *
		 * @param code The shader code.
		 */
		virtual void Reload(const std::vector<uint32>& code) = 0;

		/**
		 * Reload the shader using the shader code string.
		 *
		 * @param code The shader code.
		 */
		virtual void Reload(const std::string& code) = 0;

	public:
		/**
		 * Get the shader resources of the shader.
		 *
		 * @return The shader resources.
		 */
		const TShaderResourceMap GetShaderResources() const { return mResourceMap; }

		/**
		 * Get shader input attributes.
		 * Data to the shaders are passed via input attributes.
		 *
		 * @return The array of input attributes.
		 */
		const std::vector<ShaderAttribute> GetInputAttributes() const { return mInputAttributes; }

		/**
		 * Get shader output attributes.
		 * Data from one shader to another shader are passed using output attributes.
		 *
		 * @return The array of output attributes.
		 */
		const std::vector<ShaderAttribute> GetOutputAttributes() const { return mOutputAttributes; }

	protected:
		TShaderResourceMap mResourceMap;

		std::vector<ShaderAttribute> mInputAttributes = {};
		std::vector<ShaderAttribute> mOutputAttributes = {};

		ShaderType mType = ShaderType::Undefined;
	};
}