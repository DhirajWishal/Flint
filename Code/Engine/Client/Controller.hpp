// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/FObject.hpp"

namespace Flint
{
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
		 * @param identifier: The controller identifier.
		 */
		Controller(const std::string_view& identifier) : mIdentifier(identifier) {}

		/**
		 * Get the identifier of the controller.
		 * 
		 * @return The string view.
		 */
		const std::string_view GetIdentifier() const { return mIdentifier; }

	public:
		std::string_view mIdentifier = "";
	};
}