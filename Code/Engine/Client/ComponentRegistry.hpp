// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ComponentSystem.hpp"
#include "Components/ComponentBase.hpp"

namespace Flint
{
	class Controller;
	using TComponentMap = std::unordered_map<Controller*, std::unordered_map<std::string, std::shared_ptr<ComponentStoreInterface>>>;
	using TGlobalComponentMap = std::unordered_map<std::string, std::shared_ptr<ComponentStoreInterface>>;

	/**
	 * Flint component registry object.
	 * Registries store information about all the components used by the client application and is passed to the engine to be used.
	 * 
	 * There are two main types of components, global and private.
	 * * Global components can be accessed by any other controller.
	 * * Private components can only be accessed by the owned controller.
	 * 
	 * When updating and drawing, the global components are handled first.
	 */
	class ComponentRegistry : public FObject
	{
	public:
		ComponentRegistry() = default;

		/**
		 * Register a global component type.
		 * By registering a component type, we create the required component store.
		 *
		 * @param pController: The controller pointer.
		 */
		template<class Type>
		void RegisterGlobalComponentType()
		{
			mGlobalComponentMap[typeid(Type).name()] = std::make_shared<GlobalComponentStore<Type>>();
		}

		/**
		 * Check if the given component type is globally registered.
		 *
		 * @param pController: The controller pointer.
		 * @return The boolean value stating if its registered or not.
		 */
		template<class Type>
		const bool IsGlobalComponentTypeRegistered() const
		{
			if (mGlobalComponentMap.find(typeid(Type).name()) != mGlobalComponentMap.end())
				return true;

			return false;
		}

		/**
		 * Find a component store from the global registry.
		 * Note: This will not register any component if not present.
		 *
		 * @param pController: The controller pointer.
		 * @return The component store pointer.
		 */
		template<class Type>
		std::shared_ptr<GlobalComponentStore<Type>> FindGlobalComponentStore()
		{
			if (!IsGlobalComponentTypeRegistered<Type>())
				return nullptr;

			return  mGlobalComponentMap.at(typeid(Type).name())->StaticCast<GlobalComponentStore<Type>>().shared_from_this();
		}

		/**
		 * Find a component store from the global registry.
		 * Note: This will not register any component if not present.
		 *
		 * @return The component store pointer.
		 */
		template<class Type>
		const std::shared_ptr<GlobalComponentStore<Type>> FindGlobalComponentStore() const
		{
			if (!IsGlobalComponentTypeRegistered<Type>())
				return nullptr;

			return  mGlobalComponentMap.at(typeid(Type).name())->StaticCast<GlobalComponentStore<Type>>().shared_from_this();
		}

		/**
		 * Create a new global component.
		 *
		 * @param pController: The controller pointer.
		 * @param componentName: The name given to the component.
		 * @return The component handle.
		 */
		template<class Type, class... Args>
		const GlobalComponentHandle<Type> CreateGlobalComponent(const std::string& name , const Args&... args)
		{
			static_assert(std::is_base_of<Components::ComponentBase, Type>::value, "Provided Type is not a valid component!");

			if (!IsGlobalComponentTypeRegistered<Type>())
				RegisterGlobalComponentType<Type>();

			auto pStore = FindGlobalComponentStore<Type>();
			const UI32 index = pStore->AddComponent(name, Type(args...));

			return GlobalComponentHandle<Type>(pStore, index);
		}

		/**
		 * Get a global component.
		 * 
		 * @param name: The name of the component.
		 * @return The component pointer.
		 */
		template<class Type>
		Type* GetGlobalComponent(const std::string& name)
		{
			if (!IsGlobalComponentTypeRegistered<Type>())
				RegisterGlobalComponentType<Type>();

			auto pStore = FindGlobalComponentStore<Type>();
			return &pStore->GetComponent(name);
		}

		/**
		 * Get a global component.
		 *
		 * @param name: The name of the component.
		 * @return The component pointer.
		 */
		template<class Type>
		const Type* GetGlobalComponent(const std::string& name) const
		{
			if (!IsGlobalComponentTypeRegistered<Type>())
				RegisterGlobalComponentType<Type>();

			auto pStore = FindGlobalComponentStore<Type>();
			return &pStore->GetComponent(name);
		}

		/**
		 * Get the global component map.
		 *
		 * @return The component map.
		 */
		TGlobalComponentMap& GetGlobalComponentMap() { return mGlobalComponentMap; }

		/**
		 * Get the global component map.
		 *
		 * @return The component map.
		 */
		const TGlobalComponentMap GetGlobalComponentMap() const { return mGlobalComponentMap; }

	public:
		/**
		 * Register a component type.
		 * By registering a component type, we create the required component store.
		 *
		 * @param pController: The controller pointer.
		 */
		template<class Type>
		void RegisterComponentType(Controller* pController)
		{
			mComponentMap[pController][typeid(Type).name()] = std::make_shared<ComponentStore<Type>>();
		}

		/**
		 * Check if the given component type is registered.
		 *
		 * @param pController: The controller pointer.
		 * @return The boolean value stating if its registered or not.
		 */
		template<class Type>
		const bool IsComponentTypeRegistered(Controller* pController) const
		{
			if (mComponentMap.find(pController) != mComponentMap.end())
				if (mComponentMap.at(pController).find(typeid(Type).name()) != mComponentMap.at(pController).end())
					return true;

			return false;
		}

		/**
		 * Find a component store from the registry.
		 * Note: This will not register any component if not present.
		 *
		 * @param pController: The controller pointer.
		 * @return The component store pointer.
		 */
		template<class Type>
		std::shared_ptr<ComponentStore<Type>> FindComponentStore(Controller* pController)
		{
			if (!IsComponentTypeRegistered<Type>(pController))
				return nullptr;

			return  mComponentMap.at(pController).at(typeid(Type).name())->StaticCast<ComponentStore<Type>>().shared_from_this();
		}

		/**
		 * Find a component store from the registry.
		 * Note: This will not register any component if not present.
		 *
		 * @return The component store pointer.
		 */
		template<class Type>
		const std::shared_ptr<ComponentStore<Type>> FindComponentStore(Controller* pController) const
		{
			if (!IsComponentTypeRegistered<Type>(pController))
				return nullptr;

			return  mComponentMap.at(pController).at(typeid(Type).name())->StaticCast<ComponentStore<Type>>().shared_from_this();
		}

		/**
		 * Create a new component.
		 *
		 * @param pController: The controller pointer.
		 * @param componentName: The name given to the component.
		 * @return The component handle.
		 */
		template<class Type, class... Args>
		const ComponentHandle<Type> CreateComponent(Controller* pController, const Args&... args)
		{
			static_assert(std::is_base_of<Components::ComponentBase, Type>::value, "Provided Type is not a valid component!");

			if (!IsComponentTypeRegistered<Type>(pController))
				RegisterComponentType<Type>(pController);

			auto pStore = FindComponentStore<Type>(pController);
			const UI32 index = pStore->AddComponent(Type(args...));

			return ComponentHandle<Type>(pStore, index);
		}

		/**
		 * Get a global component.
		 *
		 * @param index: The index of the component.
		 * @return The component pointer.
		 */
		template<class Type>
		Type* GetComponent(const UI32 index)
		{
			if (!IsGlobalComponentTypeRegistered<Type>())
				RegisterGlobalComponentType<Type>();

			auto pStore = FindGlobalComponentStore<Type>();
			return &pStore->GetComponent(index);
		}

		/**
		 * Get a global component.
		 *
		 * @param index: The index of the component.
		 * @return The component pointer.
		 */
		template<class Type>
		const Type* GetComponent(const UI32 index) const
		{
			if (!IsComponentTypeRegistered<Type>())
				RegisterComponentType<Type>();

			auto pStore = FindComponentStore<Type>();
			return &pStore->GetComponent(index);
		}

		/**
		 * Get the component map.
		 *
		 * @return The component map.
		 */
		TComponentMap& GetComponentMap() { return mComponentMap; }

		/**
		 * Get the component map.
		 *
		 * @return The component map.
		 */
		const TComponentMap GetComponentMap() const { return mComponentMap; }

	protected:
		TComponentMap mComponentMap = {};
		TGlobalComponentMap mGlobalComponentMap = {};
	};
}
