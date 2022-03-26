// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ComponentHandle.hpp"

#include <unordered_map>
#include <typeindex>

namespace Flint
{
	using TComponentStore = std::unordered_map<std::type_index, std::shared_ptr<ComponentStoreInterface>>;

	/**
	 * Flint component registry.
	 * Component registries hold information about components.
	 */
	class ComponentRegistry
	{
	public:
		ComponentRegistry() = default;

		/**
		 * Check if a component type is registered.
		 *
		 * @tparam Type The type of the object.
		 * @return The boolean value.
		 */
		template<class Type>
		const bool IsRegistered() const
		{
			return mComponentStores.contains(typeid(Type));
		}

		/**
		 * Register a component to the store.
		 * 
		 * @tparam Type The type of the object.
		 */
		template<class Type>
		void RegisterComponent()
		{
			mComponentStores[typeid(Type)] = std::make_shared<ComponentStore<Type>>();
		}

		/**
		 * Get a component store from the registry.
		 * If the component store is not registered, it'll create a new store.
		 * 
		 * @tparam Type The type of the object.
		 * @return The pointer.
		 */
		template<class Type>
		std::shared_ptr<ComponentStore<Type>> GetComponentStore()
		{
			if (!IsRegistered<Type>())
				RegisterComponent<Type>();

			return std::static_pointer_cast<ComponentStore<Type>>(mComponentStores[typeid(Type)]);
		}

		/**
		 * Get a component store from the registry.
		 * If the component store is not present, it'll return nullptr.
		 *
		 * @tparam Type The type of the object.
		 * @return The pointer.
		 */
		template<class Type>
		std::shared_ptr<ComponentStore<Type>> GetComponentStore() const
		{
			if (!IsRegistered<Type>())
				return nullptr;

			return std::static_pointer_cast<ComponentStore<Type>>(mComponentStores.at(typeid(Type)));
		}



		/**
		 * Create a new component.
		 * 
		 * @param identifier The component identifier string.
		 * @param args The component's constructor arguments.
		 * @tparam Type The type of the object.
		 * @return The component handle.
		 */
		template<class Type, class... Args>
		ComponentHandle<Type> CreateComponent(const std::string& identifier, const Args&... args)
		{
			auto pStore = GetComponentStore<Type>();
			ComponentIdentifier cIdentifier(identifier);
			pStore->Store(cIdentifier, Type(args...));

			return ComponentHandle<Type>(pStore, cIdentifier);
		}

		/**
		 * Get a component from the store.
		 * 
		 * @param identifier The component identifier.
		 * @tparam Type The type of the object.
		 * @return The type pointer.
		 */
		template<class Type>
		Type* GetComponent(const std::string& identifier)
		{
			return &GetComponentStore<Type>()->Load(identifier);
		}

		/**
		 * Get all the component stores.
		 * 
		 * @return The component stores.
		 */
		TComponentStore& GetComponentStores() { return mComponentStores; }

		/**
		 * Get all the component stores.
		 *
		 * @return The component stores.
		 */
		const TComponentStore GetComponentStores() const { return mComponentStores; }

	private:
		TComponentStore mComponentStores = {};
	};
}
