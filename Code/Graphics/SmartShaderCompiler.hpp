// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Asset.hpp"
#include "GraphicsCore/Shader.hpp"

namespace Flint
{
	/**
	 * Flint smart shader compiler.
	 */
	class SmartShaderCompiler
	{
		using TIOAttribute = std::pair<InputAttributeType, UI32>;
		using TResource = std::pair<ShaderResourceType, UI32>;

	public:
		SmartShaderCompiler() = default;

		/**
		 * Add a given input attribute type.
		 *
		 * @param type The attribute type.
		 * @param location The location in the shader layout.
		 */
		void AddInput(const InputAttributeType type, const UI32 location);

		/**
		 * Add a given output attribute type.
		 *
		 * @param type The attribute type.
		 * @param location The location in the shader layout.
		 */
		void AddOutput(const InputAttributeType type, const UI32 location);

		/**
		 * Add a uniform to the shader.
		 *
		 * @param type The shader type.
		 * @param setIndex The set index of the uniform.
		 * @param binding The resource binding.
		 */
		void AddUniform(const ShaderResourceType type, const UI32 setIndex, const UI32 binding);

		/**
		 * Compile the shader.
		 * This should be done after enabling and submitting all the settings.
		 */
		void Compile();

	private:
		std::vector<TIOAttribute> mInputAttributes = {};
		std::vector<TIOAttribute> mOutputAttributes = {};

		std::unordered_map<UI32, std::vector<TResource>> mResourceSetMap = {};
	};
}