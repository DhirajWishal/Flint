// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>
#include <algorithm>

namespace Flint
{
	/**
	 * Binary map class.
	 * This class is an associative container which stores all of its data in a vector.
	 * Key-value pairs are determined using a binary search over a vector. This make iterating over this container much more faster than std::unordered_map<,>.
	 *
	 * @tparam Key The key type.
	 * @tparam Value The value type.
	 */
	template <class Key, class Value>
	class BinaryMap final
	{
	public:
		using key_type = Key;
		using value_type = Value;
		using entry_type = std::pair<key_type, value_type>;
		using container_type = std::vector<entry_type>;
		using value_container = std::vector<entry_type>;
		using iterator = typename container_type::iterator;
		using const_iterator = typename container_type::const_iterator;

		/**
		 * Comparison function used to compare when finding where an item is located.
		 *
		 * @param entry The container entry.
		 * @param key The they value to check.
		 * @return true if the entry is less than the key.
		 * @return false if the entry is grater than the key.
		 */
		static [[nodiscard]] constexpr bool comparisionFunction(const entry_type& entry, const key_type& key) { return entry.first < key; }

	public:
		/**
		 * Construct a new storage map object.
		 */
		constexpr BinaryMap() = default;

		/**
		 * Destroy the storage map object.
		 */
		~BinaryMap() = default;

		/**
		 * Insert a new key value pair to the container.
		 *
		 * @param key The key value.
		 * @param value The value to insert.
		 * @return Pair stating if it's inserted or not.
		 */
		template<class... Type>
		decltype(auto) emplace(const key_type& key, Type&&... value)
		{
			if (contains(key))
				return find(key);

			return m_Container.emplace(find(key), key, std::forward<Type>(value)...);
		}

		/**
		 * Get the value stored at a given position using the key.
		 * This will allocate a new entry if the key does not exist within the container.
		 *
		 * @param key The key value to index.
		 * @return constexpr value_type& The value type reference.
		 */
		[[nodiscard]] constexpr value_type& at(const key_type& key)
		{
			auto itr = find(key);

			// If the iterator points to the end or if the keys do not match, lets make a new entry.
			if (itr == m_Container.end() || itr->first != key)
				itr = m_Container.insert(itr, entry_type(key, value_type()));

			return itr->second;
		}

		/**
		 * Get the value stored at a given position using the key.
		 * This will allocate a new entry if the key does not exist within the container.
		 *
		 * @param key The key value to index.
		 * @return constexpr value_type& The value type reference.
		 */
		[[nodiscard]] constexpr const value_type& at(const key_type& key) const
		{
			auto itr = find(key);

			// If the iterator points to the end or if the keys do not match, lets make a new entry.
			if (itr == m_Container.end() || itr->first != key)
				itr = m_Container.insert(itr, entry_type(key, value_type()));

			return itr->second;
		}

		/**
		 * Find an iterator to where a specific key is located.
		 *
		 * @param key The key to check.
		 * @return constexpr decltype(auto) The position of that key in the container.
		 */
		[[nodiscard]] constexpr decltype(auto) find(const key_type& key) { return std::lower_bound(m_Container.begin(), m_Container.end(), key, comparisionFunction); }

		/**
		 * Find an iterator to where a specific key is located.
		 *
		 * @param key The key to check.
		 * @return constexpr decltype(auto) The position of that key in the container.
		 */
		[[nodiscard]] constexpr decltype(auto) find(const key_type& key) const { return std::lower_bound(m_Container.begin(), m_Container.end(), key, comparisionFunction); }

		/**
		 * Check if a given key is present in the container.
		 *
		 * @param key The key to check.
		 * @return true If the key is present.
		 * @return false If the key is not present.
		 */
		[[nodiscard]] constexpr bool contains(const key_type& key) const
		{
			const auto itr = find(key);
			return itr != m_Container.end() && itr->first == key;
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
		 * Subscript operator overload.
		 * This will allocate a new entry if the key does not exist within the container.
		 *
		 * @param key The key value to index.
		 * @return constexpr value_type& The value type reference.
		 */
		[[nodiscard]] constexpr value_type& operator[](const key_type& key)
		{
			auto itr = find(key);

			// If the iterator points to the end or if the keys do not match, lets make a new entry.
			if (itr == m_Container.end() || itr->first != key)
				itr = m_Container.insert(itr, entry_type(key, value_type()));

			return itr->second;
		}

		/**
		 * Get the number of types stored in the container.
		 *
		 * @return constexpr decltype(auto) The count.
		 */
		[[nodiscard]] constexpr decltype(auto) size() const { return m_Container.size(); }

		/**
		 * Get the internal data pointer.
		 *
		 * @return constexpr decltype(auto) The data pointer.
		 */
		[[nodiscard]] constexpr decltype(auto) data() const { return m_Container.data(); }

	private:
		mutable container_type m_Container = {};
		mutable std::vector<value_type> m_Values;
	};
}