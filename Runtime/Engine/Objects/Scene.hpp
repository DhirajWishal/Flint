// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <vector>
#include <unordered_map>
#include <typeindex>

#include "GameObject.hpp"

namespace Flint
{
	/**
	 * Game object store interface object.
	 */
	class GameObjectStroreInterface
	{
	public:
		/**
		 * Default constructor.
		 */
		GameObjectStroreInterface() = default;

		/**
		 * Default virtual destructor.
		 */
		virtual ~GameObjectStroreInterface() = default;
	};

	/**
	 * Game object store class.
	 * This object is just a wrapper for the std::vector class which can be accessed using the () operator.
	 */
	template<class Type>
	class GameObjectStore final : public GameObjectStroreInterface
	{
		static_assert(std::is_base_of_v<GameObject, Type>, "Invalid object! Make sure that the Type is derived from GameObject.");

	public:
		/**
		 * Default constructor.
		 */
		GameObjectStore() = default;

		/**
		 * Get the stored vector.
		 *
		 * @return The vector reference.
		 */
		std::vector<Type>& GetVector() { return mObjects; }

	private:
		std::vector<Type> mObjects = {};
	};

	/**
	 * Flint scene object.
	 * A scene object in flint stores a list of user defined game objects.
	 */
	class Scene final
	{
		using UpdateFunction = void(*)(GameObjectStroreInterface&, const uint64_t);

		using ObjectMap = std::unordered_map<std::type_index, std::unique_ptr<GameObjectStroreInterface>>;
		using ObjectUpdateHandlers = std::unordered_map<std::type_index, UpdateFunction>;

	public:
		/**
		 * Default constructor.
		 */
		Scene() = default;

		/**
		 * Check if a type is already registered in the store.
		 *
		 * @tparam Type The type of the object.
		 * @return Boolean stating if its registered or not.
		 */
		template<class Type>
		bool IsRegistered() const
		{
			return mGameObjects.contains(GetTypeIndex<Type>());
		}

		/**
		 * Register a new type.
		 * This is needed before adding a new object to the store.
		 *
		 * Note that this function is intended to be used just once for every type. Calling this more than once will lead to the whole type container being recreated.
		 * Make sure to run and check with IsRegistered() before using this function.
		 *
		 * @tparam Type The type of the object.
		 */
		template<class Type>
		void Register()
		{
			const auto index = GetTypeIndex<Type>();
			mGameObjects[index] = std::make_unique<GameObjectStore<Type>>();
			mGameObjectUpdateHandlers[index] = [](GameObjectStroreInterface& objects, const uint64_t delta)
			{
				auto& store = static_cast<GameObjectStore<Type>&>(objects);
				for (auto& object : store.GetVector())
					object.OnUpdate(delta);
			};
		}

		/**
		 * Get the type store pointer from the game object store.
		 *
		 * @tparam Type The object type.
		 * @return The game object store reference.
		 */
		template<class Type>
		GameObjectStore<Type>& GetStore() const
		{
			// First things first, check if the type is registered.
			if (!IsRegistered<Type>())
				Register<Type>();

			return static_cast<GameObjectStore<Type>&>(*mGameObjects.at(GetTypeIndex<Type>()).get());
		}

		/**
		 * Create a new game object.
		 *
		 * @param arguments The variadic arguments needed to construct the object.
		 * @tparam Type The type of the object.
		 * @tparam Arguments The variadic argument type.
		 * @return The created object pointer. This is not a permanent pointer as it might be invalid after a new creation of an object with the same type.
		 */
		template<class Type, class... Arguments>
		Type* CreateObject(Arguments&&... arguments)
		{
			// Check if the type is registered before we're able to create a new object.
			if (!IsRegistered<Type>())
				Register<Type>();

			auto& store = GetStore<Type>();
			store.GetVector().emplace_back(this, std::forward<Arguments>(arguments)...);

			return &store.GetVector().back();
		}

	public:
		/**
		 * Update all the created game objects.
		 * 
		 * @param delta The delta time difference.
		 */
		void Update(const uint64_t delta);

	private:
		/**
		 * Get the type index of a specific type.
		 *
		 * @tparam Type The type of the object.
		 * @return The type index of that type.
		 */
		template<class Type>
		constexpr std::type_index GetTypeIndex() const
		{
			return std::type_index(typeid(Type));
		}

	private:
		ObjectMap mGameObjects = {};
		ObjectUpdateHandlers mGameObjectUpdateHandlers = {};
	};
}
