// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <cstdint>
#include <vector>
#include <string>

namespace Flint
{
	/**
	 * Package entry type enum.
	 */
	enum class PackageEntryType : uint8_t
	{
		Bytes,	// Just a bunch of bytes. This is used in array types where the user can cast and unload them later on.
		String,	// String type.

		Float,	// 32 bit float.
		Double,	// 64 bit float/ double.

		Int8,	// Signed 8 bit integer.
		Int16,	// Signed 16 bit integer.
		Int32,	// Signed 32 bit integer.
		Int64,	// Signed 64 bit integer.

		Uint8,	// Unsigned 8 bit integer.
		Uint16,	// Unsigned 16 bit integer.
		Uint32,	// Unsigned 32 bit integer.
		Uint64,	// Unsigned 64 bit integer.

		Void	// None...
	};

	/**
	 * Package entry class.
	 */
	class PackageEntry final
	{
	public:
		/**
		 * Default constructor.
		 */
		PackageEntry() = default;

		/**
		 * Explicit constructor.
		 *
		 * @param size The size of the entry.
		 * @param data The data reference.
		 */
		template<class Type>
		explicit PackageEntry(PackageEntryType type, Type&& data) : m_Type(type)
		{
			m_Bytes.resize(sizeof(Type));
			std::copy_n(reinterpret_cast<const std::byte*>(&data), sizeof(Type), m_Bytes.data());
		}

		/**
		 * Explicit constructor.
		 *
		 * @param bytes The bytes to be added.
		 */
		explicit PackageEntry(std::vector<std::byte>&& bytes) : m_Bytes(std::move(bytes)), m_Type(PackageEntryType::Bytes) {}

		/**
		 * Explicit constructor.
		 *
		 * @param string The string to be set.
		 */
		explicit PackageEntry(std::string&& string) : m_Type(PackageEntryType::String)
		{
			m_Bytes.resize(string.size());
			std::copy_n(reinterpret_cast<const std::byte*>(string.data()), string.size(), m_Bytes.data());
		}

		/**
		 * Set data to the entry.
		 *
		 * @param data The data to set.
		 */
		template<class Type>
		void set(Type&& data)
		{
			m_Bytes.resize(sizeof(Type));
			std::copy_n(reinterpret_cast<const std::byte*>(&data), sizeof(Type), m_Bytes.data());
		}

		/**
		 * Set the bytes to the entry.
		 *
		 * @param bytes The bytes to be added.
		 */
		void set(std::vector<std::byte>&& bytes) { m_Bytes = std::move(bytes); }

		/**
		 * Get the stored data casted to another type.
		 *
		 * @tparam Type The type to be casted.
		 * @return The casted value reference.
		 */
		template<class Type>
		[[nodiscard]] Type& get() { return *reinterpret_cast<Type*>(m_Bytes.data()); }

		/**
		 * Get the stored data casted to another type.
		 *
		 * @tparam Type The type to be casted.
		 * @return The casted value reference.
		 */
		template<class Type>
		[[nodiscard]] const Type& get() const { return *reinterpret_cast<const Type*>(m_Bytes.data()); }

		/**
		 * Get the raw buffer pointer.
		 *
		 * @return The data pointer.
		 */
		[[nodiscard]] std::byte* getRaw() { return m_Bytes.data(); }

		/**
		 * Get the raw buffer pointer.
		 *
		 * @return The data pointer.
		 */
		[[nodiscard]] const std::byte* getRaw() const { return m_Bytes.data(); }

		/**
		 * Get the type.
		 *
		 * @return The type.
		 */
		[[nodiscard]] PackageEntryType getType() const { return m_Type; }

	private:
		std::vector<std::byte> m_Bytes;
		PackageEntryType m_Type = PackageEntryType::Void;
	};
}