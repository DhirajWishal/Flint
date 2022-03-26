// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "ComponentStore.hpp"

namespace Flint
{
	/**
	 * Component handle object.
	 * Component handles provide an easy way to access components without the need of requesting it from the component registry.
	 */
	template<class Type>
	class ComponentHandle
	{
	public:
		ComponentHandle() = default;
		ComponentHandle(const std::shared_ptr<ComponentStore<Type>>& pComponentStore, ComponentIdentifier identifier)
			: pComponentStore(pComponentStore), mIdentifier(identifier) {}

		/**
		 * Get the component from the store.
		 * We do not recommend doing this multiple times as it may result in decreased performance, but calling this function once and storing the resulting
		 * pointer on a variable and then accessing it would reduce performance overhead.
		 *
		 * @return The component pointer.
		 */
		Type* GetComponent() { return &pComponentStore->Load(mIdentifier); }

		/**
		 * Get the component from the store.
		 * We do not recommend doing this multiple times as it may result in decreased performance, but calling this function once and storing the resulting
		 * pointer on a variable and then accessing it would reduce performance overhead.
		 *
		 * @return The component pointer.
		 */
		const Type* GetComponent() const { return &pComponentStore->Load(mIdentifier); }

	public:
		/**
		 * Arrow operator.
		 * We do not recommend using this operator often as it may result in decreased performance.
		 *
		 * @return The type pointer.
		 */
		Type* operator->() { return GetComponent(); }

		/**
		 * Arrow operator.
		 * We do not recommend using this operator often as it may result in decreased performance.
		 *
		 * @return The type pointer.
		 */
		const Type* operator->() const { return GetComponent(); }

	private:
		std::shared_ptr<ComponentStore<Type>> pComponentStore = nullptr;
		ComponentIdentifier mIdentifier = {};
	};
}
