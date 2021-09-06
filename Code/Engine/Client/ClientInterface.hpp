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
		 * Register an identifier.
		 * If a class is not registered, the client will not be able to use the controller.
		 *
		 * @param identifier: The class identifier.
		 */
		void RegisterIdentifiers(const std::string& identifier);

		/**
		 * Create a new controller.
		 *
		 * @param identifier: The controller identifier.
		 * @return The created controller pointer.
		 */
		virtual std::shared_ptr<Controller> CreateController(const std::string_view& identifier) = 0;

	public:
		/**
		 * Run the client application.
		 */
		void RunClient();

	protected:
		std::vector<std::string> mRegisteredClasses = {};
	};

	/**
	 * Create client function typedef.
	 */
	typedef Flint::ClientInterface* (*TCreateClientFunction)();

	/**
	 * Destroy client function typedef.
	 */
	typedef void (*TDestroyClientFunction)(Flint::ClientInterface*);
}

/**
 * Setup the application.
 * Flint requires the applications to be built as executables, and when attached to the editor, it is loaded dynamically and can be debugged. Because of this, the
 * client application consists of two mandatory functions,
 * * CreateClient interface function.
 * * Entry point.
 *
 * The client interface function creates an instance of the client's class and returns it for the editor to use.
 * The entry point is used in program stand alone execution when deployed.
 */
#define SETUP_APPLICATION(className, ...)															\
extern "C"																							\
{																									\
	__declspec(dllexport) ::Flint::ClientInterface* CreateClient()									\
	{																								\
		return new className(__VA_ARGS__);															\
	}																								\
	 																								\
	__declspec(dllexport) void DestroyClient(::Flint::ClientInterface* pClient)						\
	{																								\
		delete pClient;																				\
	}																								\
}																									\
																									\
int main()																							\
{																									\
	className mApplication{ __VA_ARGS__ };															\
	mApplication.RunClient();																		\
}
