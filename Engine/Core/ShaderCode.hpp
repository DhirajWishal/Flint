// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>
#include <cstdint>
#include <filesystem>
#include <unordered_map>

namespace Flint
{
	/**
	 * Shader type enum.
	 */
	enum class ShaderType : uint8_t
	{
		Undefined,

		Vertex,
		TessellationControl,
		TessellationEvaluation,
		Geometry,
		Fragment,

		Compute,
	};

	/**
	 * Shader resource type enum.
	 */
	enum class ResourceType : uint8_t
	{
		Undefined,

		Sampler,
		CombinedImageSampler,
		SampledImage,
		StorageImage,
		UniformTexelBuffer,
		StorageTexelBuffer,
		UniformBuffer,
		StorageBuffer,
		DynamicUniformBuffer,
		DynamicStorageBuffer,
		InputAttachment,
		AccelerationStructure
	};

	/**
	 * Resource binding structure.
	 */
	struct ResourceBinding final
	{
		uint32_t m_Set = 0;
		uint32_t m_Binding = 0;
		uint32_t m_Count = 1;
		ResourceType m_Type = ResourceType::Undefined;
	};

	/**
	 * Shader attribute structure.
	 */
	struct ShaderAttribute final
	{
		std::string m_Name;
		uint32_t m_Location = 0;
		uint32_t m_Binding = 0;
		uint32_t m_Size = 0;
	};

	/**
	 * Push constant structure.
	 */
	struct PushConstant final
	{
		uint32_t m_Size = 0;
		uint32_t m_Offset = 0;
	};

	/**
	 * Shader code class.
	 * This class contains shader code for a single shader.
	 *
	 * Note that flint only supports SPIR-V.
	 */
	class ShaderCode final
	{
	public:
		/**
		 * Default constructor.
		 */
		ShaderCode() = default;

		/**
		 * Explicit constructor.
		 *
		 * @param path The path of the shader file.
		 * @parm type The shader type.
		 */
		explicit ShaderCode(std::filesystem::path&& path, ShaderType type);

		/**
		 * Perform reflection on the shader.
		 */
		void reflect();

		/**
		 * Get the loaded shader code.
		 *
		 * @return The shader code.
		 */
		[[nodiscard]] const std::vector<uint32_t>& getCode() const { return m_Code; }

		/**
		 * Get the input attributes.
		 *
		 * @return The attributes.
		 */
		[[nodiscard]] const std::vector<ShaderAttribute>& getInputAttributes() const { return m_InputAttributes; }

		/**
		 * Get the output attributes.
		 *
		 * @return The attributes.
		 */
		[[nodiscard]] const std::vector<ShaderAttribute>& getOutputAttributes() const { return m_OutputAttributes; }

		/**
		 * Get the push constants.
		 *
		 * @return The push constants.
		 */
		[[nodiscard]] const std::vector<PushConstant>& getPushConstants() const { return m_PushConstants; }

		/**
		 * Get the resource bindings.
		 *
		 * @return The bindings.
		 */
		[[nodiscard]] const std::unordered_map<std::string, ResourceBinding>& getBindings() const { return m_Bindings; }

		/**
		 * Get the shader type.
		 *
		 * @return The type.
		 */
		[[nodiscard]] ShaderType getType() const { return m_Type; }

	private:
		std::vector<uint32_t> m_Code;

		std::vector<ShaderAttribute> m_InputAttributes;
		std::vector<ShaderAttribute> m_OutputAttributes;

		std::vector<PushConstant> m_PushConstants;

		std::unordered_map<std::string, ResourceBinding> m_Bindings;

		ShaderType m_Type = ShaderType::Undefined;
	};
}