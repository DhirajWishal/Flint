// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <unordered_map>
#include <string>

namespace Flint
{
	/**
	 * Static storage class.
	 * This class can be stored to dynamically store objects which can be accessed statically.
	 *
	 * @tparam Type The value type.
	 */
	template<class Type>
	class StaticStorage final
	{
		/**
		 * Default constructor.
		 */
		StaticStorage() = default;

		/**
		 * Default destructor.
		 */
		~StaticStorage() = default;

		/**
		 * Get the internal storage.
		 *
		 * @return The storage map.
		 */
		[[nodiscard]] std::unordered_map<std::string, Type>& getStorage() { return m_Storage; }

		/**
		 * Get the internal storage.
		 *
		 * @return The storage map.
		 */
		[[nodiscard]] const std::unordered_map<std::string, Type>& getStorage() const { return m_Storage; }

	public:
		/**
		 * Get the instance.
		 *
		 * @return The instance.
		 */
		static StaticStorage<Type>& Get() { static StaticStorage<Type> storage; return storage; }

		/**
		 * Store a value in the storage.
		 *
		 * @param identifier The value's identifier string.
		 * @param value The value to set.
		 */
		static void Set(const std::string& identifier, Type&& value) { Get().getStorage()[identifier] = std::move(value); }

		/**
		 * Store a value in the storage.
		 *
		 * @param identifier The value's identifier string.
		 * @param value The value to set.
		 */
		static void Set(const std::string& identifier, const Type& value) { Get().getStorage()[identifier] = value; }

		/**
		 * Get a value from the storage.
		 *
		 * @param identifier The value identifier.
		 * @return The value reference.
		 */
		static Type& Get(const std::string& identifier) { return Get().getStorage()[identifier]; }

		/**
		 * Check if a value with the identifier contains in the storage.
		 *
		 * @param identifier The value identifier.
		 * @return Boolean value.
		 */
		static bool Contains(const std::string& identifier) { return Get().getStorage().contains(identifier); }

	private:
		std::unordered_map<std::string, Type> m_Storage;
	};
}