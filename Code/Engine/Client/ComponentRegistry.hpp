// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Components/MaterialPipeline.hpp"
#include "ComponentSystem.hpp"

#include <type_traits>

namespace Flint
{
	using TComponentMap = std::unordered_map<std::string, std::shared_ptr<ComponentStoreInterface>>;

	/**
	 * Flint component registry object.
	 * Registries store information about all the components used by the client application and is passed to the engine to be used.
	 */
	class ComponentRegistry : public FObject
	{
	public:
		ComponentRegistry() = default;

		/**
		 * Register a component type.
		 * By registering a component type, we create the required component store.
		 */
		template<class Type>
		void RegisterComponentType()
		{
			mComponentMap[typeid(Type).name()] = std::make_shared<ComponentStore<Type>>();
		}

		/**
		 * Check if the given component type is registered.
		 *
		 * @return The boolean value stating if its registered or not.
		 */
		template<class Type>
		const bool IsComponentTypeRegistered() const
		{
			return mComponentMap.find(typeid(Type).name()) != mComponentMap.end();
		}

		/**
		 * Find a component store from the registry.
		 * Note: This will not register any component if not present.
		 *
		 * @return The component store pointer.
		 */
		template<class Type>
		std::shared_ptr<ComponentStore<Type>> FindComponentStore()
		{
			return  mComponentMap.at(typeid(Type).name())->StaticCast<ComponentStore<Type>>().shared_from_this();
		}

		/**
		 * Find a component store from the registry.
		 * Note: This will not register any component if not present.
		 *
		 * @return The component store pointer.
		 */
		template<class Type>
		const std::shared_ptr<ComponentStore<Type>> FindComponentStore() const
		{
			return  mComponentMap.at(typeid(Type).name())->StaticCast<ComponentStore<Type>>().shared_from_this();
		}

		/**
		 * Create a new component.
		 *
		 * @param controllerName: The name of the controller which created this component.
		 * @param componentName: The name given to the component.
		 * @return The component handle.
		 */
		template<class Type>
		const ComponentHandle<Type> CreateComponent(const std::string_view& controllerName, const std::string_view& componentName)
		{
			if (!IsComponentTypeRegistered<Type>())
				RegisterComponentType<Type>();

			auto pStore = FindComponentStore<Type>();
			const UI32 index = pStore->AddComponent(Type(controllerName, componentName));

			return ComponentHandle<Type>(pStore, index);
		}

		/**
		 * Get the component map.
		 * 
		 * @return The component map.
		 */
		const TComponentMap GetComponentMap() const { return mComponentMap; }

	protected:
		TComponentMap mComponentMap = {};
	};
}
