// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/BinaryMap.hpp"
#include "Core/Hasher.hpp"

namespace Flint
{
	/**
	 * Component identifier structure.
	 * These structures are just hashes of strings which allow us to easily compare and check if a given component is present within the component store.
	 * This also results in reduced memory usage and also improve performance by not needing to allocate new memory to copy a string.
	 */
	struct ComponentIdentifier
	{
		ComponentIdentifier() = default;
		explicit ComponentIdentifier(const std::string_view& string) : m_StringHash(Hasher::HashDataBlock(string.data(), string.size())), m_StringLength(string.size()) {}

		auto operator<=>(const ComponentIdentifier& other) const = default;

		uint64_t m_StringHash = 0;
		uint64_t m_StringLength = 0;
	};

	/**
	 * Component store interface object.
	 */
	class ComponentStoreInterface {};

	/**
	 * Component store object.
	 * These objects are used to store the components of a given type.
	 */
	template<class Type>
	class ComponentStore final : public ComponentStoreInterface
	{
	public:
		using Iterator		= typename BinaryMap<ComponentIdentifier, Type>::Iterator;
		using ConstIterator = typename BinaryMap<ComponentIdentifier, Type>::ConstIterator;

		/**
		 * Default constructor.
		 */
		ComponentStore() = default;

		/**
		 * Store a component in the store.
		 *
		 * @param identifier The component identifier.
		 * @param data The data to be stored.
		 */
		void Store(ComponentIdentifier identifier, const Type& data) { m_Components[identifier] = data; }

		/**
		 * Store a component in the store.
		 *
		 * @param identifier The component identifier.
		 * @param data The data to be stored.
		 */
		void Store(ComponentIdentifier identifier, Type&& data) { m_Components[identifier] = std::move(data); }

		/**
		 * Load a component from the store.
		 *
		 * @param identifier THe component's identifier.
		 * @return The type reference.
		 */
		Type& Load(ComponentIdentifier identifier) { return m_Components.At(identifier); }

		/**
		 * Load a component from the store.
		 *
		 * @param identifier THe component's identifier.
		 * @return The const type.
		 */
		const Type Load(ComponentIdentifier identifier) const { return m_Components.At(identifier); }

		/**
		 * Remove an component from the store.
		 *
		 * @param identifier The identifier of the component to remove.
		 */
		void Remove(ComponentIdentifier identifier) { m_Components.Remove(identifier); }

		/**
		 * Check if a component is present.
		 *
		 * @param identifier The identifier of the component.
		 * @return The boolean value.
		 */
		const bool IsPresent(ComponentIdentifier identifier) const { return m_Components.Find(identifier) != m_Components.end(); }

		/**
		 * Get the size of the store.
		 *
		 * @return The size.
		 */
		const uint64_t Size() const { return m_Components.Size(); }

	public:
		/**
		 * Get the begin iterator.
		 * 
		 * @return The iterator.
		 */
		Iterator begin() { return m_Components.begin(); }

		/**
		 * Get the begin iterator.
		 *
		 * @return The iterator.
		 */
		ConstIterator begin() const { return m_Components.begin(); }

		/**
		 * Get the end iterator.
		 *
		 * @return The iterator.
		 */
		Iterator end() { return m_Components.end(); }

		/**
		 * Get the end iterator.
		 *
		 * @return The iterator.
		 */
		ConstIterator end() const { return m_Components.end(); }

	private:
		BinaryMap<ComponentIdentifier, Type> m_Components = {};
	};
}