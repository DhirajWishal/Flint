// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>
#include <cstdint>
#include <filesystem>

namespace Flint
{
	namespace Backend
	{
		/**
		 * Shader code class.
		 * This class contains information about a single shader code.
		 *
		 * Since Flint only uses SPIR-V, make sure that the submitted shader data are SPIR-V.
		 */
		class ShaderCode final
		{
		public:
			/**
			 * Explicit constructor.
			 *
			 * @param code The shader code in a vector.
			 */
			explicit ShaderCode(std::vector<uint32_t>&& code);

			/**
			 * Explicit constructor.
			 *
			 * @param file The shader source file.
			 */
			explicit ShaderCode(std::filesystem::path&& file);

			/**
			 * Get the size of the shader code.
			 * Note that this will only return the number of elements in the internal array. To find the actual byte size, multiply by sizeof(uint32_t).
			 *
			 * @return The size.
			 */
			[[nodiscard]] uint32_t getSize() const { return m_Code.size(); }

			/**
			 * Check if the shader code is empty.
			 *
			 * @return Whether or not the shader code is empty.
			 */
			[[nodiscard]] bool empty() const { return m_Code.empty(); }

			/**
			 * Get the internally stored shader code.
			 *
			 * @return The shader code vector.
			 */
			[[nodiscard]] const std::vector<uint32_t>& get() const { return m_Code; }

		private:
			std::vector<uint32_t> m_Code;
		};
	}
}