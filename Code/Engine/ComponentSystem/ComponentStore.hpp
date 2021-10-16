// Copyright 2021 Dhiraj Wishal
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
		ComponentIdentifier(const std::string& string) : mStringHash(Hasher::HashDataBlock(string.data(), string.size())) {}

		const bool operator<(const ComponentIdentifier& other) const { return mStringHash < other.mStringHash; }
		const bool operator==(const ComponentIdentifier& other) const { return mStringHash == other.mStringHash; }
		const bool operator!=(const ComponentIdentifier& other) const { return mStringHash != other.mStringHash; }

		UI64 mStringHash = 0;
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
		using Iterator		= typename BinaryMap<ComponentIdentifier, Type>::Iterator;
		using ConstIterator = typename BinaryMap<ComponentIdentifier, Type>::ConstIterator;

	public:
		ComponentStore() = default;

		/**
		 * Store a component in the store.
		 *
		 * @param identifier The component identifier.
		 * @param data The data to be stored.
		 */
		void Store(ComponentIdentifier identifier, const Type& data) { mComponents[identifier] = data; }

		/**
		 * Store a component in the store.
		 *
		 * @param identifier The component identifier.
		 * @param data The data to be stored.
		 */
		void Store(ComponentIdentifier identifier, Type&& data) { mComponents[identifier] = std::move(data); }

		/**
		 * Load a component from the store.
		 *
		 * @param identifier THe component's identifier.
		 * @return The type reference.
		 */
		Type& Load(ComponentIdentifier identifier) { return mComponents.At(identifier); }

		/**
		 * Load a component from the store.
		 *
		 * @param identifier THe component's identifier.
		 * @return The const type.
		 */
		const Type Load(ComponentIdentifier identifier) const { return mComponents.At(identifier); }

		/**
		 * Remove an component from the store.
		 *
		 * @param identifier The identifier of the component to remove.
		 */
		void Remove(ComponentIdentifier identifier) { mComponents.Remove(identifier); }

		/**
		 * Check if a component is present.
		 *
		 * @param identifier The identifier of the component.
		 * @return The boolean value.
		 */
		const bool IsPresent(ComponentIdentifier identifier) const { return mComponents.Find(identifier) != mComponents.end(); }

		/**
		 * Get the size of the store.
		 *
		 * @return The size.
		 */
		const UI64 Size() const { return mComponents.Size(); }

	public:
		/**
		 * Get the begin iterator.
		 * 
		 * @return The iterator.
		 */
		Iterator begin() { return mComponents.begin(); }

		/**
		 * Get the begin iterator.
		 *
		 * @return The iterator.
		 */
		ConstIterator begin() const { return mComponents.begin(); }

		/**
		 * Get the end iterator.
		 *
		 * @return The iterator.
		 */
		Iterator end() { return mComponents.end(); }

		/**
		 * Get the end iterator.
		 *
		 * @return The iterator.
		 */
		ConstIterator end() const { return mComponents.end(); }

	private:
		BinaryMap<ComponentIdentifier, Type> mComponents = {};
	};
}