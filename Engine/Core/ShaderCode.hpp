// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Types.hpp"

#include <filesystem>
#include <unordered_map>

namespace Flint
{
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
	 * This class only supports SPIR-V.
	 *
	 * Note that inputs, and output attributes are required to be in the ascending order and cannot jump numbers like 1, 4, 5, ..., for example in GLSL:
	 * layout(location = 0) in vec4 diffuseAlbedo;	// Starts with 0
	 * layout(location = 1) in vec2 texCoord;
	 * layout(location = 2) in vec3 cameraSpaceNormal;
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
		 * Set the shader entry point name.
		 * The entry point will be called by the device to run the shader. By default it is main.
		 *
		 * @param name The name of the entry point function.
		 */
		void setEntryPoint(std::string&& name) { m_EntryPoint = std::move(name); }

		/**
		 * Get the shader entry point.
		 *
		 * @return The entry point name.
		 */
		[[nodiscard]] std::string_view getEntryPoint() const { return m_EntryPoint; }

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
		/**
		 * Perform reflection on the shader.
		 */
		void reflect();

	private:
		std::vector<uint32_t> m_Code;

		std::vector<ShaderAttribute> m_InputAttributes;
		std::vector<ShaderAttribute> m_OutputAttributes;

		std::vector<PushConstant> m_PushConstants;

		std::unordered_map<std::string, ResourceBinding> m_Bindings;

		std::string m_EntryPoint = "main";

		ShaderType m_Type = ShaderType::Undefined;
	};
}