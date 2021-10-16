// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DataType.hpp"
#include <vector>

namespace Flint
{
	/**
	 * Flint binary map object.
	 * Binary map object basically acts as a hash map, but uses a compare function (less than for now) to find a key. This is faster in iterations as it uses a vector
	 * to store all the entries.
	 */
	template<class Key, class Value>
	class BinaryMap
	{
	public:
		using KeyValue			= std::pair<Key, Value>;

		using Iterator			= typename std::vector<KeyValue>::iterator;
		using ConstIterator		= typename std::vector<KeyValue>::const_iterator;

	public:
		BinaryMap() = default;

		/**
		 * Find the lower bound key iterator.
		 *
		 * @param key The key to be searched for.
		 * @return The position iterator.
		 */
		Iterator Find(const Key& key)
		{
			Iterator begin = mEntries.begin();
			UI64 count = mEntries.size(), step = 0;

			while (count > 0)
			{
				auto iterator = begin;
				step = count / 2;
				std::advance(iterator, step);

				if (iterator->first < key)
					begin = ++iterator, count -= step + 1;
				else
					count = step;
			}

			return begin;
		}

		/**
		 * Find the lower bound key iterator.
		 *
		 * @param key The key to be searched for.
		 * @return The position iterator.
		 */
		ConstIterator Find(const Key& key) const
		{
			ConstIterator begin = mEntries.begin();
			UI64 count = mEntries.size(), step = 0;

			while (count > 0)
			{
				auto iterator = begin;
				step = count / 2;
				std::advance(iterator, step);

				if (iterator->first < key)
					begin = ++iterator, count -= step + 1;
				else
					count = step;
			}

			return begin;
		}

		/**
		 * Get a particular value at a given position.
		 *
		 * @param key The key to search for.
		 * @return The value reference.
		 */
		Value& At(const Key& key)
		{
			auto iterator = Find(key);
			if (iterator == mEntries.end() || iterator->first != key)
			{
				mEntries.insert(iterator, KeyValue(key, Value()));
				return At(key);
			}

			return iterator->second;
		}

		/**
		 * Get a particular value at a given position.
		 *
		 * @param key The key to search for.
		 * @return The value reference.
		 */
		const Value At(const Key& key) const
		{
			auto iterator = Find(key);
			if (iterator == mEntries.end() || iterator->first != key)
				return Value();

			return iterator->second;
		}

		/**
		 * Insert a key value pair into the map.
		 *
		 * @param key The key of the value.
		 * @param value The value to insert.
		 */
		void Insert(const Key& key, const Value& value)
		{
			mEntries.insert(Find(key), KeyValue(key, value));
		}

		/**
		 * Remove a given entry using its key.
		 *
		 * @param key The key to remove.
		 */
		void Remove(const Key& key)
		{
			auto iterator = Find(key);
			if (iterator != mEntries.end() && iterator->first == key)
				mEntries.erase(iterator);
		}

	public:
		/**
		 * Begin iterator function.
		 *
		 * @return The iterator.
		 */
		Iterator begin() { return mEntries.begin(); }

		/**
		 * Begin iterator function.
		 *
		 * @return The iterator.
		 */
		ConstIterator begin() const { return mEntries.begin(); }

		/**
		 * End iterator function.
		 *
		 * @return The iterator.
		 */
		Iterator end() { return mEntries.end(); }

		/**
		 * End iterator function.
		 *
		 * @return The iterator.
		 */
		ConstIterator end() const { return mEntries.end(); }

		/**
		 * Get the size of the map.
		 *
		 * @return The size.
		 */
		const UI64 Size() const { return mEntries.size(); }

		/**
		 * Get the capacity of the map.
		 *
		 * @return The capacity.
		 */
		const UI64 Capacity() const { return mEntries.capacity(); }

	public:
		/**
		 * Subscript operator overload.
		 *
		 * @param key The key to search.
		 * @return The value reference.
		 */
		Value& operator[](const Key& key)
		{
			auto iterator = Find(key);
			if (iterator == mEntries.end() || iterator->first != key)
			{
				mEntries.insert(iterator, KeyValue(key, Value()));
				return operator[](key);
			}

			return iterator->second;
		}

	private:
		std::vector<KeyValue> mEntries = {};
	};
}