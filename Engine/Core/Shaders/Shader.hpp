// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/FObject.hpp"
#include "Core/Types.hpp"

#include <vector>
#include <filesystem>

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
	 * Push constant structure.
	 */
	struct PushConstant final
	{
		uint32_t m_Size = 0;
		uint32_t m_Offset = 0;
	};

	/**
	 * Shader class.
	 * This is the base class for all the shaders.
	 */
	class Shader : public FObject
	{
	public:
		/**
		 * Default constructor.
		 */
		Shader() = default;

		/**
		 * Explicit constructor.
		 * This will load the shader data from a file.
		 *
		 * @param file The shader source file.
		 */
		explicit Shader(std::filesystem::path&& file);

		/**
		 * Default destructor.
		 */
		virtual ~Shader() = default;

		/**
		 * Set the entry point of the shader.
		 * This function is called by the GPU when running the shader.
		 *
		 * @param name The entry point function's name.
		 */
		void setEntryPoint(std::string&& name) noexcept;

		/**
		 * Get the entry point name.
		 *
		 * @return The name.
		 */
		[[nodiscard]] std::string_view getEntryPoint() const noexcept { return m_EntryPoint; }

		/**
		 * Get the shader code.
		 *
		 * @return The shader code.
		 */
		[[nodiscard]] const std::vector<uint32_t>& getCode() const noexcept { return m_Code; }

		/**
		 * Get the push constants.
		 *
		 * @return The push constants.
		 */
		[[nodiscard]] const std::vector<PushConstant>& getPushConstants() const noexcept { return m_PushConstants; }

		/**
		 * Get the resource bindings.
		 *
		 * @return The bindings.
		 */
		[[nodiscard]] const std::vector<ResourceBinding>& getBindings() const noexcept { return m_Bindings; }

	private:
		/**
		 * Perform reflection over the loaded shader code.
		 */
		void reflect();

	protected:
		std::vector<uint32_t> m_Code;
		std::vector<PushConstant> m_PushConstants;
		std::vector<ResourceBinding> m_Bindings;

		std::string m_EntryPoint = "main";
	};
}