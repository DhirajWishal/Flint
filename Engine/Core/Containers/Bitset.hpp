// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <array>
#include <cstdint>
#include <execution>

namespace Flint
{
	/**
	 * @brief Bit set class.
	 * This class is used to store a `Bits` number of bits. The main difference between this and the `std::bitset` is that, the `std::bitset` uses `bool`s to store
	 * its values whereas this uses actual bits to store values.
	 *
	 * @tparam Bits The number of bits.
	 */
	template <uint64_t Bits>
	class Bitset final
	{
		/**
		 * @brief Bit field structure.
		 * This contains 1 byte worth of 8 bits which can be accessed separately.
		 */
		struct bit_field
		{
			union
			{
				struct
				{
					bool m_A : 1;
					bool m_B : 1;
					bool m_C : 1;
					bool m_D : 1;
					bool m_E : 1;
					bool m_F : 1;
					bool m_G : 1;
					bool m_H : 1;
				};

				uint8_t m_Value; // This variable is there to sum up the bits in the bit field and to easily compare and do other operations.
			};

			/**
			 * @brief Is equal to operator.
			 *
			 * @param other The other bit field.
			 * @return true if this is equal to the other.
			 * @return false if this is not equal to the other.
			 */
			constexpr [[nodiscard]] bool operator==(const bit_field other) const { return m_Value == other.m_Value; }

			/**
			 * @brief Is not equal to operator.
			 *
			 * @param other The other bit field.
			 * @return true if this is not equal to the other.
			 * @return false if this is equal to the other.
			 */
			constexpr [[nodiscard]] bool operator!=(const bit_field other) const { return m_Value != other.m_Value; }

			/**
			 * @brief Less than operator.
			 *
			 * @param other The other bit field to compare.
			 * @return true if this is less than the other.
			 * @return false if this is grater than or equal to the other.
			 */
			constexpr [[nodiscard]] bool operator<(const bit_field other) const { return m_Value < other.m_Value; }

			/**
			 * @brief Grater than or equal operator.
			 *
			 * @param other The other bit field to compare.
			 * @return true if this is grater than or equal the other.
			 * @return false if this is less than the other.
			 */
			constexpr [[nodiscard]] bool operator>=(const bit_field other) const { return m_Value >= other.m_Value; }

			/**
			 * @brief Bitwise and operator.
			 *
			 * @param other The other bit field.
			 * @return constexpr uint8_t The bitwise AND value.
			 */
			constexpr [[nodiscard]] uint8_t operator&(const bit_field other) const { return m_Value & other.m_Value; }
		};

		/**
		 * @brief Resolve the array size.
		 *
		 * @tparam size The user defined size.
		 * @return constexpr uint64_t The array size to set.
		 */
		template <uint64_t size>
		static consteval [[nodiscard]] uint64_t resolveArraySize()
		{
			if constexpr (size % 8 == 0)
				return size / 8;
			else
				return (size / 8) + 1;
		}

	public:
		/**
		 * @brief Default constructor.
		 */
		constexpr Bitset() = default;

		/**
		 * @brief Get the size of the internal byte array.
		 *
		 * @return constexpr uint64_t The size.
		 */
		constexpr [[nodiscard]] uint64_t size() const { return resolveArraySize<Bits>(); }

		/**
		 * @brief Get the indexable capacity of the internal byte array.
		 *
		 * @return constexpr uint64_t The capacity.
		 */
		constexpr [[nodiscard]] uint64_t capacity() const { return m_Bytes.size() * 8; }

		/**
		 * @brief Test a given position to check if the bit value is 1 or 0.
		 *
		 * @param pos The position of the bit.
		 * @return true if the bit is 1.
		 * @return false if the bit is 0.
		 */
		constexpr [[nodiscard]] bool test(const uint64_t pos) const
		{
			const auto field = m_Bytes[pos / 8];
			switch (pos % 8)
			{
			case 1:
				return field.m_B;

			case 2:
				return field.m_C;

			case 3:
				return field.m_D;

			case 4:
				return field.m_E;

			case 5:
				return field.m_F;

			case 6:
				return field.m_G;

			case 7:
				return field.m_H;

			default:
				return field.m_A;
			}
		}

		/**
		 * @brief Toggle a bit to true.
		 *
		 * @param pos The bit position to toggle.
		 * @param value The value to set.
		 */
		constexpr void toggle(const uint64_t pos, const bool value)
		{
			const auto index = pos / 8;
			switch (pos % 8)
			{
			case 1:
				m_Bytes[index].m_B = value;
				break;

			case 2:
				m_Bytes[index].m_C = value;
				break;

			case 3:
				m_Bytes[index].m_D = value;
				break;

			case 4:
				m_Bytes[index].m_E = value;
				break;

			case 5:
				m_Bytes[index].m_F = value;
				break;

			case 6:
				m_Bytes[index].m_G = value;
				break;

			case 7:
				m_Bytes[index].m_H = value;
				break;

			default:
				m_Bytes[index].m_A = value;
				break;
			}
		}

		/**
		 * @brief Toggle a bit to true.
		 *
		 * @param pos The bit position to toggle.
		 */
		constexpr void toggleTrue(const uint64_t pos) { toggle(pos, true); }

		/**
		 * @brief Toggle a bit to false.
		 *
		 * @param pos The bit position to toggle.
		 */
		constexpr void toggleFalse(const uint64_t pos) { toggle(pos, true); }

		/**
		 * @brief Get the container that's actually holding the data.
		 *
		 * @return constexpr decltype(auto) The container.
		 */
		constexpr [[nodiscard]] decltype(auto) container() const { return m_Bytes; }

		/**
		 * @brief Index a single bit using the position of it.
		 *
		 * @param pos The position to access.
		 * @return true if the bit value is 1.
		 * @return false if the bit value is 0.
		 */
		constexpr [[nodiscard]] bool operator[](const uint64_t pos) const { return test(pos); }

		/**
		 * @brief Not equal to operator.
		 *
		 * @param other The other bit set.
		 * @return true if this bit set is not equal to the other.
		 * @return false if this bit set is equal to the other.
		 */
		constexpr [[nodiscard]] bool operator!=(const Bitset other) const
		{
			return std::equal(std::execution::unseq, m_Bytes.begin(), m_Bytes.end(), other.m_Bytes.begin(), [](const auto lhs, const auto rhs) { return lhs != rhs; });
		}

		/**
		 * @brief Less than operator.
		 *
		 * @param other The other bit set.
		 * @return true if this bit set is less than the other.
		 * @return false if this bit set is grater than or equal to the other.
		 */
		constexpr [[nodiscard]] bool operator<(const Bitset other) const
		{
			return std::equal(std::execution::unseq, m_Bytes.begin(), m_Bytes.end(), other.m_Bytes.begin(), [](const auto lhs, const auto rhs) { return lhs < rhs; });
		}

		/**
		 * @brief Logical AND operator.
		 * This will internally perform a bitwise AND to all the bytes stored.
		 *
		 * @param other The other bit set.
		 * @return true if all the bits are present.
		 * @return false if there are no bits in common.
		 */
		constexpr [[nodiscard]] bool operator&&(const Bitset other) const
		{
			return !std::equal(std::execution::unseq, m_Bytes.begin(), m_Bytes.end(), other.m_Bytes.begin(), [](const auto lhs, const auto rhs) { return !((lhs & rhs) == rhs.m_Value); });
		}

		/**
		 * @brief Logical OR operator.
		 * This will internally perform a bitwise AND to all the bytes stored.
		 *
		 * @param other The other bit set.
		 * @return true if there is at least one bit in common.
		 * @return false if there are no bits in common.
		 */
		constexpr [[nodiscard]] bool operator||(const Bitset other) const
		{
			return !std::equal(std::execution::unseq, m_Bytes.begin(), m_Bytes.end(), other.m_Bytes.begin(), [](const auto lhs, const auto rhs) { return !((lhs & rhs) > 0); });
		}

	private:
		std::array<bit_field, resolveArraySize<Bits>()> m_Bytes = {};
	};
} // namespace inventory