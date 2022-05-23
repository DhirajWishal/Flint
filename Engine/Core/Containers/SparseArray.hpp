// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>
#include <algorithm>

#ifdef FLINT_PLATFORM_WINDOWS
#	include <execution>

#endif

namespace Flint
{
	/**
	 * Integral type concept.
	 * This concept will only accept integral type.
	 *
	 * @tparam Type The type to be checked.
	 */
	template <class Type>
	concept index_type = std::is_unsigned_v<Type>;

	/**
	 * Sparse array object.
	 * This object consists of three main arrays. dense_vector, which contains the actual data. sparse_vector(s) which contains the actual indexes of the data in the dense_vector, and another
	 * vector containing the reusable indexes. Here we use another vector for this because it's easier to index it rather than iterating over the sparse index array and finding out which
	 * index is usable.
	 *
	 * @tparam Type The type of data to store.
	 * @tparam Index The integral type used to index. Default is uint64_t.
	 */
	template <class Type, index_type Index = uint64_t>
	class SparseArray final
	{
		using dense_vector = std::vector<Type>;
		using sparse_vector = std::vector<Index>;
		using availability_vector = std::vector<bool>;
		static constexpr Index invalid_index = -1;

		dense_vector m_DenseArray = {};		  // This is where we store the actual data.
		sparse_vector m_SparseArray = {};	  // This is where we store the indexes.
		sparse_vector m_ReusableIndexes = {}; // This is where we store the reusable indexes.

		/**
		 * Reducer function used to reduce the index by one.
		 *
		 * @param index The index to reduce.
		 * @return constexpr Index The return of it.
		 */
		constexpr static Index reducer(const Index& index) { return index - 1; }

	public:
		using value_type = Type;
		using index_type = Index;
		using iterator = typename dense_vector::iterator;
		using const_iterator = typename dense_vector::const_iterator;

		/**
		 * Default constructor.
		 */
		constexpr SparseArray() = default;

		/**
		 * Default destructor.
		 */
		~SparseArray() = default;

		/**
		 * Emplace a new element to the dense array.
		 *
		 * @tparam Types The variadic argument types.
		 * @param data The data to emplace.
		 * @return constexpr std::pair<Index, std::reference_wrapper<Type>> The index position of the emplaced data and the emplaced data reference.
		 */
		template <class... Types>
		[[nodiscard]] constexpr std::pair<Index, Type*> emplace(Types &&...data)
		{
			auto index = get_index();
			auto& emplaced = m_DenseArray.emplace_back(std::forward<Types>(data)...);
			update_sparse_vector(index, m_DenseArray.size() - 1);

			return std::make_pair(index, &emplaced);
		}

		/**
		 * Remove a single entry from the dense array using it's index.
		 * Note that this operation is quite slow, and should not be done in places such as clearing this array.
		 *
		 * @param index The index to remove.
		 */
		constexpr void remove(Index index)
		{
			// Remove the data from the dense array.
			const auto indexToErase = m_SparseArray[index];
			m_DenseArray.erase(m_DenseArray.begin() + indexToErase);

			// If the dense array is empty, we can clear the other two vectors.
			if (m_DenseArray.empty())
			{
				m_SparseArray.clear();
				m_ReusableIndexes.clear();
			}

			// If the index is in the last position of the sparse array, we can clear it along with the rest of the invalid indexes.
			else if (index == m_SparseArray.size() - 1)
			{
				m_SparseArray.pop_back();
				shrink_to_fit();
			}

			// Else we can add the index as a reusable index, and subtract one from the sparse index from that point on.
			else
			{
				m_ReusableIndexes.emplace_back(index);

				auto beginItr = m_SparseArray.begin() + index;

#ifdef FLINT_PLATFORM_WINDOWS
				std::transform(std::execution::unseq, beginItr, m_SparseArray.end(), beginItr, reducer);

#else
				std::transform(beginItr, m_SparseArray.end(), beginItr, reducer);

#endif
			}
		}

		/**
		 * Clear this container.
		 */
		constexpr void clear()
		{
			m_DenseArray.clear();
			m_SparseArray.clear();
			m_ReusableIndexes.clear();
		}

		/**
		 * Get an element at a given position.
		 *
		 * @param index The index to access.
		 * @return constexpr Type& The data reference.
		 */
		[[nodiscard]] constexpr Type& at(const Index& index) { return m_DenseArray[m_SparseArray[index]]; }

		/**
		 * Get an element at a given position.
		 *
		 * @param index The index to access.
		 * @return constexpr const Type& The data reference.
		 */
		[[nodiscard]] constexpr const Type& at(const Index& index) const { return m_DenseArray[m_SparseArray[index]]; }

		/**
		 * Get the begin iterator.
		 *
		 * @return constexpr decltype(auto) The iterator.
		 */
		[[nodiscard]] constexpr decltype(auto) begin() { return m_DenseArray.begin(); }

		/**
		 * Get the end iterator.
		 *
		 * @return constexpr decltype(auto) The iterator.
		 */
		[[nodiscard]] constexpr decltype(auto) end() { return m_DenseArray.end(); }

		/**
		 * Get the begin iterator.
		 *
		 * @return constexpr decltype(auto) The iterator.
		 */
		[[nodiscard]] constexpr decltype(auto) begin() const { return m_DenseArray.begin(); }

		/**
		 * Get the end iterator.
		 *
		 * @return constexpr decltype(auto) The iterator.
		 */
		[[nodiscard]] constexpr decltype(auto) end() const { return m_DenseArray.end(); }

		/**
		 * Check if a given index is present in the container.
		 *
		 * @param index The index to check.
		 * @return constexpr true if the index is present in the container.
		 * @return constexpr false if the index is not present in the container.
		 */
		[[nodiscard]] constexpr bool contains(const Index& index) const
		{
			if (index < m_SparseArray.size())
				return m_SparseArray[index] > invalid_index;

			return false;
		}

		/**
		 * Subscript operator.
		 *
		 * @param index The index to access.
		 * @return constexpr Type& The data reference.
		 */
		[[nodiscard]] constexpr Type& operator[](const Index& index) { return m_DenseArray[m_SparseArray[index]]; }

		/**
		 * Subscript operator.
		 *
		 * @param index The index to access.
		 * @return constexpr const Type& The data reference.
		 */
		[[nodiscard]] constexpr const Type& operator[](const Index& index) const { return m_DenseArray[m_SparseArray[index]]; }

	private:
		/**
		 * Get the next available index.
		 *
		 * @return constexpr Index The index.
		 */
		[[nodiscard]] constexpr Index get_index()
		{
			if (!m_ReusableIndexes.empty())
			{
				const Index index = m_ReusableIndexes.back();
				m_ReusableIndexes.pop_back();

				return index;
			}
			else
				return static_cast<Index>(m_DenseArray.size());
		}

		/**
		 * update the sparse vector.
		 *
		 * @param givenIndex The index given to store the data.
		 * @param storedIndex The actual index of the stored data.
		 */
		constexpr void update_sparse_vector(const Index givenIndex, const Index storedIndex)
		{
			if (givenIndex < m_SparseArray.size())
				m_SparseArray[givenIndex] = storedIndex;

			else
				m_SparseArray.emplace_back(storedIndex);
		}

		/**
		 * Shrink the sparse array to fit the optimal size.
		 */
		constexpr void shrink_to_fit()
		{
			for (auto itr = m_SparseArray.end() - 1; itr != m_SparseArray.begin(); --itr)
			{
				if (*itr > invalid_index)
				{
					m_SparseArray.erase(itr, m_SparseArray.end());
					break;
				}
			}
		}
	};
}