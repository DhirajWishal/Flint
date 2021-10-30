// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/FObject.hpp"
#include "ComponentSystem/ComponentRegistry.hpp"

namespace Flint
{
	class ClientInterface;

	/**
	 * Update specification structure.
	 * This describes specifications each controller needs to follow when updating.
	 */
	struct UpdateSpecification
	{
		bool bSkipMousePosition = false;
	};

	/**
	 * Flint controller object.
	 * This is the base class for all the supported client objects.
	 */
	class Controller : public FObject
	{
	public:
		/**
		 * Construct the controller.
		 * 
		 * @param identifier The controller identifier.
		 * @param pClientInterface The client interface pointer which created the controller.
		 */
		Controller(const std::string_view& identifier, ClientInterface* pClientInterface) : mIdentifier(identifier), pClientInterface(pClientInterface) {}

		/**
		 * Get the identifier of the controller.
		 * 
		 * @return The string view.
		 */
		const std::string_view GetIdentifier() const { return mIdentifier; }

		/**
		 * On update function.
		 * 
		 * @param specification The update specification.
		 * @param delta The time delta.
		 */
		virtual void OnUpdate(const uint64 delta, const UpdateSpecification specification) = 0;

	public:
		std::string_view mIdentifier = "";
		ClientInterface* pClientInterface = nullptr;
	};
}