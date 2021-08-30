// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Controller.hpp"

#include <vector>

namespace Flint
{
	/**
	 * Flint client interface class.
	 * This class is the base class for the client application and all application MUST inherit from this.
	 */
	class ClientInterface : public FObject
	{
	public:
		ClientInterface() = default;

		/**
		 * Get all the class identifiers from the client application.
		 * 
		 * @return The vector of registered classes.
		 */
		std::vector<std::string_view> GetClassIdentifiers() const;

		/**
		 * Create a new controller.
		 * 
		 * @param identifier: The controller identifier.
		 * @return The created controller pointer.
		 */
		virtual std::shared_ptr<Controller> CreateController(const std::string_view& identifier) = 0;

	protected:
		std::vector<std::string> mRegisteredClasses = {};
	};
}

/**
 * Export the client class to the editor.
 * Every application which wishes to use the Flint Editor is required to export their class using this macro. If not the class would not be visible to the editor.
 */
#define EXPORT_TO_EDITOR(className)	extern "C" __declspec(dllexport) std::shared_ptr<::Flint::ClientInterface> CreateClient() { return std::make_shared<className>(); }
