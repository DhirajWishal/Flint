// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DataTypes.h"

namespace Flint
{
	/**
	 * Vector Set for Flint.
	 * Vector set is used as a std::unordered_map/ std::set but in this case, we store it in a vector. The entries
	 * are sorted by the Key value, and we binary search out way finding the entry.
	 *
	 * @tparam Key: The key type.
	 * @tparam Value: The value which is mapped to the Key type.
	 * @param Compare: The compare struct. Default is std::less<Key>.
	 */
	template<class Key, class Value, class Compare = std::less<Key>>
	class VectorSet : private Compare {
		using ValuePair = std::pair<Key, Value>;
		using Container = std::vector<ValuePair>;

		typedef typename Container::iterator Iterator;
		typedef typename Container::const_iterator ConstIterator;
		typedef typename Container::reverse_iterator ReverseIterator;
		typedef typename Container::const_reverse_iterator ConstReverseIterator;

	public:
		/**
		 * Default constructor.
		 */
		VectorSet();

		/**
		 * Copy constructor.
		 *
		 * @param other: The other container.
		 */
		VectorSet(const VectorSet& other);

		/**
		 * Move constructor.
		 *
		 * @param other: The other container.
		 */
		VectorSet(VectorSet&& other);

		/**
		 * Construct the container using another container's iterator.
		 *
		 * @param begin: The begin iterator.
		 * @param end: The end iterator.
		 */
		VectorSet(Iterator begin, Iterator end);

		/**
		 * Construct the container using another container's iterator.
		 *
		 * @param begin: The begin iterator.
		 * @param end: The end iterator.
		 */
		VectorSet(ConstIterator begin, ConstIterator end);

		/**
		 * Construct the container using another container's iterator.
		 *
		 * @param begin: The begin iterator.
		 * @param end: The end iterator.
		 */
		VectorSet(ConstReverseIterator begin, ConstReverseIterator end);

		/**
		 * Construct the container using another container's iterator.
		 *
		 * @param begin: The begin iterator.
		 * @param end: The end iterator.
		 */
		VectorSet(ReverseIterator begin, ReverseIterator end);

		/**
		 * Default destructor.
		 */
		~VectorSet();

		/**
		 * Insert an entry to the container.
		 *
		 * @param value: The value pair to be inserted.
		 * @return Boolean value stating whether its inserted or not.
		 */
		bool Insert(const ValuePair& value);

		/**
		 * Insert an entry to the container.
		 *
		 * @param value: The value pair to be inserted.
		 * @return Boolean value stating whether its inserted or not.
		 */
		bool Insert(ValuePair&& value);

		/**
		 * Insert content from another container to this.
		 *
		 * @param where: The place to insert the new entries to in the container.
		 * @param begin: The begin iterator of the other container.
		 * @param end: The end iterator of the other container.
		 */
		void Insert(Iterator where, Iterator begin, Iterator end);

		/**
		 * Insert content from another container to this.
		 *
		 * @param where: The place to insert the new entries to in the container.
		 * @param begin: The begin iterator of the other container.
		 * @param end: The end iterator of the other container.
		 */
		void Insert(Iterator where, ConstIterator begin, ConstIterator end);

		/**
		 * Insert content from another container to this.
		 *
		 * @param where: The place to insert the new entries to in the container.
		 * @param begin: The begin iterator of the other container.
		 * @param end: The end iterator of the other container.
		 */
		void Insert(Iterator where, ReverseIterator begin, ReverseIterator end);

		/**
		 * Insert content from another container to this.
		 *
		 * @param where: The place to insert the new entries to in the container.
		 * @param begin: The begin iterator of the other container.
		 * @param end: The end iterator of the other container.
		 */
		void Insert(Iterator where, ConstReverseIterator begin, ConstReverseIterator end);

		/**
		 * Clear the container.
		 */
		void Clear();

	public:
		/**
		 * Get the number of entries stored in the vector.
		 *
		 * @return The number of entries.
		 */
		UI64 Size() const;

		/**
		 * Get the capacity of the vector.
		 *
		 * @return The number of entries till the next allocation.
		 */
		UI64 Capacity() const;

		/**
		 * Check if the container is empty.
		 *
		 * @return Boolean value.
		 */
		bool IsEmpty() const;

		/**
		 * Get the begin iterator.
		 *
		 * @return The std::vector::iterator.
		 */
		Iterator Begin();

		/**
		 * Get the end iterator.
		 *
		 * @return The std::vector::iterator.
		 */
		Iterator End();

		/**
		 * Get the begin iterator.
		 *
		 * @return The std::vector::const_iterator.
		 */
		ConstIterator Begin() const;

		/**
		 * Get the end iterator.
		 *
		 * @return The std::vector::const_iterator.
		 */
		ConstIterator End() const;

		/**
		 * Get the begin iterator.
		 *
		 * @return The std::vector::const_iterator.
		 */
		ConstIterator CBegin() const;

		/**
		 * Get the end iterator.
		 *
		 * @return The std::vector::const_iterator.
		 */
		ConstIterator CEnd() const;

		/**
		 * Get the begin iterator.
		 *
		 * @return The std::vector::reverse_iterator.
		 */
		ReverseIterator RBegin();

		/**
		 * Get the end iterator.
		 *
		 * @return The std::vector::reverse_iterator.
		 */
		ReverseIterator REnd();

		/**
		 * Get the begin iterator.
		 *
		 * @return The std::vector::const_reverse_iterator.
		 */
		ConstReverseIterator CRBegin() const;

		/**
		 * Get the end iterator.
		 *
		 * @return The std::vector::const_reverse_iterator.
		 */
		ConstReverseIterator CREnd() const;

	public:
		/**
		 * Index operator.
		 * This returns the value stored/ mapped to a certain key. The returned value can be modified.
		 *
		 * @param key: The key to be searched for.
		 * @return The value reference.
		 */
		Value& operator[](const Key& key);

		/**
		 * Index operator.
		 * This returns the value stored/ mapped to a certain key.
		 *
		 * @param key: The key to be searched for.
		 * @return The value.
		 */
		Value operator[](const Key& key) const;

		/**
		 * Assignment operator (copy).
		 *
		 * @param other: The other vector set.
		 * @return The vector set reference.
		 */
		VectorSet& operator=(const VectorSet& other);

		/**
		 * Assignment operator (move).
		 *
		 * @param other: The other vector set.
		 * @return The vector set reference.
		 */
		VectorSet& operator=(VectorSet&& other);

		/**
		 * Is equal operator.
		 *
		 * @param other: The other vector set to be compared with.
		 * @return Boolean value.
		 */
		bool operator==(const VectorSet& other) const;

		/**
		 * Is less than operator.
		 *
		 * @param other: The other vector set to be compared with.
		 * @return Boolean value.
		 */
		bool operator<(const VectorSet& other) const;

		/**
		 * Is less than or equal operator.
		 *
		 * @param other: The other vector set to be compared with.
		 * @return Boolean value.
		 */
		bool operator<=(const VectorSet& other) const;

		/**
		 * Is grater than operator.
		 *
		 * @param other: The other vector set to be compared with.
		 * @return Boolean value.
		 */
		bool operator>(const VectorSet& other) const;

		/**
		 * Is grater than or equal operator.
		 *
		 * @param other: The other vector set to be compared with.
		 * @return Boolean value.
		 */
		bool operator>=(const VectorSet& other) const;

	protected:
		/**
		 * Get the lower bound of an entry.
		 *
		 * @param key: The key to be checked.
		 * @return The iterator of the index.
		 */
		Iterator LowerBound(const Key& key);

		/**
		 * Get the lower bound of an entry.
		 *
		 * @param key: The key to be checked.
		 * @return The iterator of the index.
		 */
		ConstIterator LowerBound(const Key& key) const;

	protected:
		Container mEntries = {};
	};

	template<class Key, class Value, class Compare>
	inline VectorSet<Key, Value, Compare>::VectorSet()
	{
	}

	template<class Key, class Value, class Compare>
	inline VectorSet<Key, Value, Compare>::VectorSet(const VectorSet& other) : mEntries(other.mEntries)
	{
	}

	template<class Key, class Value, class Compare>
	inline VectorSet<Key, Value, Compare>::VectorSet(VectorSet&& other) : mEntries(std::move(other.mEntries))
	{
	}

	template<class Key, class Value, class Compare>
	inline VectorSet<Key, Value, Compare>::VectorSet(Iterator begin, Iterator end) : mEntries(begin, end)
	{
	}

	template<class Key, class Value, class Compare>
	inline VectorSet<Key, Value, Compare>::VectorSet(ConstIterator begin, ConstIterator end) : mEntries(begin, end)
	{
	}

	template<class Key, class Value, class Compare>
	inline VectorSet<Key, Value, Compare>::VectorSet(ConstReverseIterator begin, ConstReverseIterator end) : mEntries(begin, end)
	{
	}

	template<class Key, class Value, class Compare>
	inline VectorSet<Key, Value, Compare>::VectorSet(ReverseIterator begin, ReverseIterator end) : mEntries(begin, end)
	{
	}

	template<class Key, class Value, class Compare>
	inline VectorSet<Key, Value, Compare>::~VectorSet()
	{
	}

	template<class Key, class Value, class Compare>
	inline bool VectorSet<Key, Value, Compare>::Insert(const ValuePair& value)
	{
		Iterator itr = LowerBound(value.first);
		if (itr == End())
		{
			mEntries.insert(itr, value);
			return true;
		}

		return false;
	}

	template<class Key, class Value, class Compare>
	inline bool VectorSet<Key, Value, Compare>::Insert(ValuePair&& value)
	{
		Iterator itr = LowerBound(value.first);
		if (itr == End())
		{
			mEntries.insert(itr, std::move(value));
			return true;
		}

		return false;
	}

	template<class Key, class Value, class Compare>
	inline void VectorSet<Key, Value, Compare>::Insert(Iterator where, Iterator begin, Iterator end)
	{
		mEntries.insert(where, begin, end);
	}

	template<class Key, class Value, class Compare>
	inline void VectorSet<Key, Value, Compare>::Insert(Iterator where, ConstIterator begin, ConstIterator end)
	{
		mEntries.insert(where, begin, end);
	}

	template<class Key, class Value, class Compare>
	inline void VectorSet<Key, Value, Compare>::Insert(Iterator where, ReverseIterator begin, ReverseIterator end)
	{
		mEntries.insert(where, begin, end);
	}

	template<class Key, class Value, class Compare>
	inline void VectorSet<Key, Value, Compare>::Insert(Iterator where, ConstReverseIterator begin, ConstReverseIterator end)
	{
		mEntries.insert(where, begin, end);
	}

	template<class Key, class Value, class Compare>
	inline void VectorSet<Key, Value, Compare>::Clear()
	{
		mEntries.clear();
	}

	template<class Key, class Value, class Compare>
	inline UI64 VectorSet<Key, Value, Compare>::Size() const
	{
		return mEntries.size();
	}

	template<class Key, class Value, class Compare>
	inline UI64 VectorSet<Key, Value, Compare>::Capacity() const
	{
		return mEntries.capacity();
	}

	template<class Key, class Value, class Compare>
	inline bool VectorSet<Key, Value, Compare>::IsEmpty() const
	{
		return mEntries.empty();
	}

	template<class Key, class Value, class Compare>
	inline typename VectorSet<Key, Value, Compare>::Iterator VectorSet<Key, Value, Compare>::Begin()
	{
		return mEntries.begin();
	}

	template<class Key, class Value, class Compare>
	inline typename VectorSet<Key, Value, Compare>::Iterator VectorSet<Key, Value, Compare>::End()
	{
		return mEntries.end();
	}

	template<class Key, class Value, class Compare>
	inline typename VectorSet<Key, Value, Compare>::ConstIterator VectorSet<Key, Value, Compare>::Begin() const
	{
		return mEntries.begin();
	}

	template<class Key, class Value, class Compare>
	inline typename VectorSet<Key, Value, Compare>::ConstIterator VectorSet<Key, Value, Compare>::End() const
	{
		return mEntries.end();
	}

	template<class Key, class Value, class Compare>
	inline typename VectorSet<Key, Value, Compare>::ConstIterator VectorSet<Key, Value, Compare>::CBegin() const
	{
		return mEntries.cbegin();
	}

	template<class Key, class Value, class Compare>
	inline typename VectorSet<Key, Value, Compare>::ConstIterator VectorSet<Key, Value, Compare>::CEnd() const
	{
		return mEntries.cend();
	}

	template<class Key, class Value, class Compare>
	inline typename VectorSet<Key, Value, Compare>::ReverseIterator VectorSet<Key, Value, Compare>::RBegin()
	{
		return mEntries.rbegin();
	}

	template<class Key, class Value, class Compare>
	inline typename VectorSet<Key, Value, Compare>::ReverseIterator VectorSet<Key, Value, Compare>::REnd()
	{
		return mEntries.rend();
	}

	template<class Key, class Value, class Compare>
	inline typename VectorSet<Key, Value, Compare>::ConstReverseIterator VectorSet<Key, Value, Compare>::CRBegin() const
	{
		return mEntries.crbegin();
	}

	template<class Key, class Value, class Compare>
	inline typename VectorSet<Key, Value, Compare>::ConstReverseIterator VectorSet<Key, Value, Compare>::CREnd() const
	{
		return mEntries.crend();
	}

	template<class Key, class Value, class Compare>
	inline Value& VectorSet<Key, Value, Compare>::operator[](const Key& key)
	{
		Iterator itr = LowerBound(key);
		if (itr == End() || itr->first != key)
		{
			UI64 index = itr - Begin();
			mEntries.insert(itr, ValuePair(key, Value()));
			return mEntries[index].second;
		}

		return itr->second;
	}

	template<class Key, class Value, class Compare>
	inline Value VectorSet<Key, Value, Compare>::operator[](const Key& key) const
	{
		ConstIterator itr = LowerBound(key);
		if (itr == End() || itr->first != key)
			return Value();

		return itr->second;
	}

	template<class Key, class Value, class Compare>
	inline VectorSet<Key, Value, Compare>& VectorSet<Key, Value, Compare>::operator=(const VectorSet& other)
	{
		mEntries = other.mEntries;
		return *this;
	}

	template<class Key, class Value, class Compare>
	inline VectorSet<Key, Value, Compare>& VectorSet<Key, Value, Compare>::operator=(VectorSet&& other)
	{
		mEntries = std::move(other.mEntries);
		return *this;
	}

	template<class Key, class Value, class Compare>
	inline bool VectorSet<Key, Value, Compare>::operator==(const VectorSet& other) const
	{
		return mEntries == other.mEntries;
	}

	template<class Key, class Value, class Compare>
	inline bool VectorSet<Key, Value, Compare>::operator<(const VectorSet& other) const
	{
		return mEntries < other.mEntries;
	}

	template<class Key, class Value, class Compare>
	inline bool VectorSet<Key, Value, Compare>::operator<=(const VectorSet& other) const
	{
		return mEntries <= other.mEntries;
	}

	template<class Key, class Value, class Compare>
	inline bool VectorSet<Key, Value, Compare>::operator>(const VectorSet& other) const
	{
		return mEntries > other.mEntries;
	}

	template<class Key, class Value, class Compare>
	inline bool VectorSet<Key, Value, Compare>::operator>=(const VectorSet& other) const
	{
		return mEntries >= other.mEntries;
	}

	template<class Key, class Value, class Compare>
	inline typename VectorSet<Key, Value, Compare>::Iterator VectorSet<Key, Value, Compare>::LowerBound(const Key& key)
	{
		I64 counter = Size();
		I64 counter2 = counter / 2;
		Iterator begin = Begin();
		Iterator itr = begin + counter2;

		Compare compare = {};
		while (counter > 0)
		{
			if (compare(itr->first, key))
				begin = ++itr, counter -= counter2 + 1;
			else
				counter = counter2;
			counter2 = counter / 2;
			itr = begin + counter2;
		}

		return begin;
	}

	template<class Key, class Value, class Compare>
	inline typename VectorSet<Key, Value, Compare>::ConstIterator VectorSet<Key, Value, Compare>::LowerBound(const Key& key) const
	{
		I64 counter = Size();
		I64 counter2 = counter / 2;
		ConstIterator begin = Begin();
		ConstIterator itr = begin + counter2;

		Compare compare = {};
		while (counter > 0)
		{
			if (compare(itr->first, key))
				begin = ++itr, counter -= counter2 + 1;
			else
				counter = counter2;
			counter2 = counter / 2;
			itr = begin + counter2;
		}

		return begin;
	}
}