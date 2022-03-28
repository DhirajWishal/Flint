// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DataType.hpp"

namespace Flint
{
	/**
	 * Flint sparse array.
	 * Sparse arrays are just like normal arrays, but when removed, the size does not shrink.
	 */
	template<class Type>
	class SparseArray
	{
	public:
		SparseArray() = default;
		~SparseArray() = default;

		/**
		 * Add a new element to the array.
		 *
		 * @param data The data to add.
		 * @return The data index.
		 */
		const uint32_t push_back(const Type& data)
		{
			m_Entries.emplace_back(data);
			return m_CurrentIndex++;
		}

		/**
		 * Add a new element to the array.
		 *
		 * @param data The data to add.
		 * @return The data index.
		 */
		const uint32_t push_back(Type&& data)
		{
			m_Entries.emplace_back(std::move(data));
			return m_CurrentIndex++;
		}

		/**
		 * Get data from a given index.
		 *
		 * @param index The index position of the data to access.
		 * @return The type reference.
		 */
		Type& at(const uint32_t index) { return m_Entries[index]; }

		/**
		 * Get data from a given index.
		 *
		 * @param index The index position of the data to access.
		 * @return The const type.
		 */
		const Type at(const uint32_t index) const { return m_Entries[index]; }

		/**
		 * Remove an element form the array.
		 * This does not necessarily remove the entry from the array, but will call its destructor.
		 *
		 * @param index The index of the element to remove.
		 */
		void remove(const uint32_t index) { std::_Destroy_in_place(at(index)); }

		/**
		 * Get the size of the array.
		 *
		 * @return The size of the array.
		 */
		const uint64_t size() const { return m_Entries.size(); }

		/**
		 * Get the capacity of the array.
		 *
		 * @return The capacity of the array.
		 */
		const uint64_t capacity() const { return m_Entries.capacity(); }

		/**
		 * Check if the array is empty.
		 *
		 * @return The boolean value.
		 */
		const bool empty() const { return m_Entries.empty(); }

		/**
		 * Get the begin iterator.
		 *
		 * @return The vector iterator.
		 */
		typename std::vector<Type>::iterator begin() { return m_Entries.begin(); }

		/**
		 * Get the end iterator.
		 *
		 * @return The vector iterator.
		 */
		typename std::vector<Type>::iterator end() { return m_Entries.end(); }

		/**
		 * Get the begin iterator.
		 *
		 * @return The vector iterator.
		 */
		typename std::vector<Type>::const_iterator begin() const { return m_Entries.begin(); }

		/**
		 * Get the end iterator.
		 *
		 * @return The vector iterator.
		 */
		typename std::vector<Type>::const_iterator end() const { return m_Entries.end(); }

	public:
		/**
		 * Subscript operator.
		 * 
		 * @param index The index of the element.
		 * @return The element.
		 */
		Type& operator[](const uint32_t index)
		{
			return at(index);
		}

		/**
		 * Subscript operator.
		 *
		 * @param index The index of the element.
		 * @return The element.
		 */
		const Type operator[](const uint32_t index) const
		{
			return at(index);
		}

	private:
		std::vector<Type> m_Entries;
		uint32_t m_CurrentIndex = 0;
	};
}