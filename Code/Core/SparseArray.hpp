// Copyright 2021 Dhiraj Wishal
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
		const UI32 push_back(const Type& data)
		{
			mEntries.push_back(data);
			return mCurrentIndex++;
		}

		/**
		 * Add a new element to the array.
		 *
		 * @param data The data to add.
		 * @return The data index.
		 */
		const UI32 push_back(Type&& data)
		{
			mEntries.push_back(std::move(data));
			return mCurrentIndex++;
		}

		/**
		 * Get data from a given index.
		 *
		 * @param index The index position of the data to access.
		 * @return The type reference.
		 */
		Type& at(const UI32 index) { return mEntries[index]; }

		/**
		 * Get data from a given index.
		 *
		 * @param index The index position of the data to access.
		 * @return The const type.
		 */
		const Type at(const UI32 index) const { return mEntries[index]; }

		/**
		 * Remove an element form the array.
		 * This does not necessarily remove the entry from the array, but will call its destructor.
		 *
		 * @param index The index of the element to remove.
		 */
		void remove(const UI32 index) { std::_Destroy_in_place(at(index)); }

		/**
		 * Get the size of the array.
		 *
		 * @return The size of the array.
		 */
		const UI64 size() const { return mEntries.size(); }

		/**
		 * Get the capacity of the array.
		 *
		 * @return The capacity of the array.
		 */
		const UI64 capacity() const { return mEntries.capacity(); }

		/**
		 * Check if the array is empty.
		 *
		 * @return The boolean value.
		 */
		const bool empty() const { return mEntries.empty(); }

		/**
		 * Get the begin iterator.
		 *
		 * @return The vector iterator.
		 */
		typename std::vector<Type>::iterator begin() { return mEntries.begin(); }

		/**
		 * Get the end iterator.
		 *
		 * @return The vector iterator.
		 */
		typename std::vector<Type>::iterator end() { return mEntries.end(); }

		/**
		 * Get the begin iterator.
		 *
		 * @return The vector iterator.
		 */
		typename std::vector<Type>::const_iterator begin() const { return mEntries.begin(); }

		/**
		 * Get the end iterator.
		 *
		 * @return The vector iterator.
		 */
		typename std::vector<Type>::const_iterator end() const { return mEntries.end(); }

	public:
		/**
		 * Subscript operator.
		 * 
		 * @param index The index of the element.
		 * @return The element.
		 */
		Type& operator[](const UI32 index)
		{
			return at(index);
		}

		/**
		 * Subscript operator.
		 *
		 * @param index The index of the element.
		 * @return The element.
		 */
		const Type operator[](const UI32 index) const
		{
			return at(index);
		}

	private:
		std::vector<Type> mEntries;
		UI32 mCurrentIndex = 0;
	};
}