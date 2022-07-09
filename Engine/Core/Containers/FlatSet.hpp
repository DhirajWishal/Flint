// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>
#include <algorithm>

namespace Flint
{
	/**
	 * Flat map class.
	 * This class is just a set, but using a vector compared to a binary tree using nodes.
	 *
	 * @tparam Type The value type.
	 */
	template <class Type>
	class FlatSet final
	{
	public:
		using value_type = Type;
		using container_type = std::vector<value_type>;
		using iterator = typename container_type::iterator;
		using const_iterator = typename container_type::const_iterator;

		/**
		 * Comparison function used to compare when finding where an item is located.
		 *
		 * @param entry The container entry.
		 * @param value The they value to check.
		 * @return true if the entry is less than the value.
		 * @return false if the entry is grater than the value.
		 */
		static [[nodiscard]] constexpr bool comparison_function(const value_type& entry, const value_type& value) { return entry < value; }

	public:
		/**
		 * Construct a new storage map object.
		 */
		constexpr FlatSet() = default;

		/**
		 * Destroy the storage map object.
		 */
		~FlatSet() = default;

		/**
		 * Insert a new value to the container.
		 *
		 * @param value The value to be inserted.
		 * @return constexpr decltype(auto) The resulting pair.
		 */
		[[nodiscard]] constexpr decltype(auto) insert(const value_type& value)
		{
			auto itr = find(value);
			if (itr != m_Container.end() && *itr == value)
				return std::make_pair(false, itr);

			itr = m_Container.emplace(itr, value);
			return std::make_pair(true, itr);
		}

		/**
		 * Find an iterator to where a specific value is located.
		 *
		 * @param value The value to check.
		 * @return constexpr decltype(auto) The position of that value in the container.
		 */
		[[nodiscard]] constexpr decltype(auto) find(const value_type& value) { return std::lower_bound(m_Container.begin(), m_Container.end(), value, comparison_function); }

		/**
		 * Find an iterator to where a specific value is located.
		 *
		 * @param value The value to check.
		 * @return constexpr decltype(auto) The position of that value in the container.
		 */
		[[nodiscard]] constexpr decltype(auto) find(const value_type& value) const { return std::lower_bound(m_Container.begin(), m_Container.end(), value, comparison_function); }

		/**
		 * Check if a given value is present in the container.
		 *
		 * @param value The value to check.
		 * @return true If the value is present.
		 * @return false If the value is not present.
		 */
		[[nodiscard]] constexpr bool contains(const value_type& value) const
		{
			const auto itr = find(value);
			return itr != m_Container.end() && *itr == value;
		}

		/**
		 * Remove a value from the container.
		 *
		 * @param value The value to remove.
		 */
		constexpr void remove(const value_type& value)
		{
			const auto itr = find(value);
			if (itr != m_Container.end() && *itr == value)
				m_Container.erase(itr);
		}

		/**
		 * Get the begin iterator.
		 *
		 * @return constexpr decltype(auto) Begin iterator.
		 */
		[[nodiscard]] constexpr decltype(auto) begin() { return m_Container.begin(); }

		/**
		 * Get the end iterator.
		 *
		 * @return constexpr decltype(auto) End iterator.
		 */
		[[nodiscard]] constexpr decltype(auto) end() { return m_Container.end(); }

		/**
		 * Get the begin iterator.
		 *
		 * @return constexpr decltype(auto) Begin iterator.
		 */
		[[nodiscard]] constexpr decltype(auto) begin() const { return m_Container.begin(); }

		/**
		 * Get the end iterator.
		 *
		 * @return constexpr decltype(auto) End iterator.
		 */
		[[nodiscard]] constexpr decltype(auto) end() const { return m_Container.end(); }

		/**
		 * Get the number of types stored in the container.
		 *
		 * @return constexpr decltype(auto) The count.
		 */
		[[nodiscard]] constexpr decltype(auto) size() const { return m_Container.size(); }

	private:
		container_type m_Container = {};
	};
}