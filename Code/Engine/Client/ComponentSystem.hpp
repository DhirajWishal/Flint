// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/SparseArray.hpp"

namespace Flint
{
	/**
	 * Component store interface.
	 */
	class ComponentStoreInterface : public FObject {};

	/**
	 * Component store object.
	 * This object will store the actual components.
	 */
	template<class Type>
	class ComponentStore : public ComponentStoreInterface, public std::enable_shared_from_this<ComponentStore<Type>>
	{
	public:
		ComponentStore() = default;

		/**
		 * Add a component to the store.
		 * 
		 * @param component: The component to add.
		 * @return The component index.
		 */
		const UI32 AddComponent(const Type& component) { return mComponents.push_back(component); }

		/**
		 * Add a component to the store.
		 *
		 * @param component: The component to add.
		 * @return The component index.
		 */
		const UI32 AddComponent(Type&& component) { return mComponents.push_back(std::move(component)); }

		/**
		 * Get a component from the store.
		 * 
		 * @param index: The index of the component.
		 * @return The component.
		 */
		Type& GetComponent(const UI32 index) { return mComponents[index]; }

		/**
		 * Get a component from the store.
		 *
		 * @param index: The index of the component.
		 * @return The component.
		 */
		const Type GetComponent(const UI32 index) const { return mComponents[index]; }

		/**
		 * Get the size of the component array.
		 * 
		 * @return The size of the array.
		 */
		const UI64 Size() const { return mComponents.size(); }

		/**
		 * Check if the array is empty.
		 * 
		 * @return The boolean value.
		 */
		const bool IsEmpty() const { return mComponents.empty(); }

		/**
		 * Get the array from the store.
		 * 
		 * @return The sparse array.
		 */
		SparseArray<Type>& GetArray() { return mComponents; }

		/**
		 * Get the array from the store.
		 *
		 * @return The sparse array.
		 */
		const SparseArray<Type> GetArray() const { return mComponents; }

	private:
		SparseArray<Type> mComponents = {};
	};

	class ComponentRegistry;

	/**
	 * Component handle object.
	 * These handles can be used to easily retrieve the actual component without the need of consulting the registry.
	 */
	template<class Type>
	class ComponentHandle
	{
		friend ComponentRegistry;

		/**
		 * Construct the handle.
		 *
		 * @param pStore: The component store.
		 * @param index: The component index.
		 */
		ComponentHandle(const std::shared_ptr<ComponentStore<Type>>& pStore, const UI32 index) : pComponentStore(pStore), mIndex(index) {}

	public:
		ComponentHandle() = default;

		/**
		 * Get the component.
		 *
		 * @return The component pointer.
		 */
		Type* GetComponent() { return &pComponentStore->GetComponent(mIndex); }

		/**
		 * Get the component.
		 *
		 * @return The component pointer.
		 */
		const Type* GetComponent() const { return &pComponentStore->GetComponent(mIndex); }

	public:
		/**
		 * Arrow operator.
		 * Note: We don't recommend using this regularly as it is quite slow. Use GetComponent() to get the pointer to a local variable and then use that instead.
		 * 
		 * @return The component pointer.
		 */
		Type* operator->() { return GetComponent(); }

		/**
		 * Arrow operator.
		 * Note: We don't recommend using this regularly as it is quite slow. Use GetComponent() to get the pointer to a local variable and then use that instead.
		 *
		 * @return The component pointer.
		 */
		const Type* operator->() const { return GetComponent(); }

	private:
		/**
		 * Get the component index.
		 *
		 * @return The index.
		 */
		const UI32 GetIndex() const { return mIndex; }

	private:
		std::shared_ptr<ComponentStore<Type>> pComponentStore;
		UI32 mIndex = 0;
	};
}
