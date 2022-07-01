// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Buffer.hpp"

#include <variant>
#include <unordered_map>

namespace Flint
{
	/**
	 * Mesh binding table class.
	 * This is used to bind resources to different resource locations.
	 */
	class MeshBindingTable final
	{
	public:
		/**
		 * Default constructor
		 */
		MeshBindingTable() = default;

		/**
		 * Bind a buffer to the required binding.
		 *
		 * @param binding The buffer's binding.
		 * @param pBuffer The buffer to bind.
		 */
		void bind(uint32_t binding, const std::shared_ptr<Buffer>& pBuffer);

		/**
		 * Generate the hash for this table.
		 *
		 * @return The hash value for this table.
		 */
		[[nodiscard]] uint64_t generateHash() const;

		/**
		 * Get the bound buffers.
		 *
		 * @return The buffer map.
		 */
		[[nodiscard]] const std::unordered_map<uint32_t, std::shared_ptr<Buffer>> getBuffers() const { return m_pBuffers; }

	private:
		std::unordered_map<uint32_t, std::shared_ptr<Buffer>> m_pBuffers;
	};
}