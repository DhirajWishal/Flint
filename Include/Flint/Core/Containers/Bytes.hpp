// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>
#include <string>

namespace Flint
{
	/**
	 * Bytes class.
	 * This class can be used to store byte information.
	 */
	class Bytes final
	{
	public:
		/**
		 * Default constructor.
		 */
		constexpr Bytes() = default;

		/**
		 * Explicit constructor.
		 *
		 * @tparam Iterator The iterator type.
		 * @param first The first iterator (begin).
		 * @param last The last iterator (end).
		 */
		template<class Iterator>
		constexpr explicit Bytes(Iterator&& first, Iterator&& last) : m_Bytes(std::move(first), std::move(last)) {}

		/**
		 * Default destructor.
		 */
		~Bytes() = default;

		/**
		 * Insert a string to the storage.
		 *
		 * @param string The string to insert.
		 */
		void insert(std::string_view string);

		/**
		 * Insert an integer value to the storage.
		 *
		 * @param value The value to insert.
		 */
		void insert(uint64_t value);

		/**
		 * Get the number of bytes stored in the container.
		 *
		 * @return The count.
		 */
		[[nodiscard]] uint64_t getSize() const noexcept { return m_Bytes.size(); }

		/**
		 * Check if the container is empty.
		 *
		 * @return Whether the container is empty or not.
		 */
		[[nodiscard]] bool isEmpty() const noexcept { return m_Bytes.empty(); }

		/**
		 * Get the internal storage.
		 *
		 * @return The byte storage.
		 */
		[[nodiscard]] std::vector<std::byte>& getStorage() { return m_Bytes; }

		/**
		 * Get the internal storage.
		 *
		 * @return The byte storage.
		 */
		[[nodiscard]] const std::vector<std::byte>& getStorage() const { return m_Bytes; }

	private:
		std::vector<std::byte> m_Bytes;
	};
}