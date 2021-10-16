// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Controller.hpp"
#include "Graphics/GeometryStoreManager.hpp"

namespace Flint
{
	/**
	 * Flint client interface class.
	 * This class is the base class for the client application and all application MUST inherit from this.
	 * 
	 * Once the client is created, the first registered controller will be created. After that the controllers are supposed to create other controllers and keep the logic flow.
	 */
	class ClientInterface : public ComponentRegistry, public GeometryStoreManager
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
		 * @param identifier The controller identifier.
		 * @return The created controller pointer.
		 */
		virtual std::shared_ptr<Controller> CreateController(const std::string_view& identifier) = 0;

	public:
		/**
		 * Activate a controller.
		 * 
		 * @identifier: The identifier of the controller.
		 */
		void ActivateController(const std::string_view& identifier);

		/**
		 * Deactivate a controller.
		 *
		 * @identifier: The identifier of the controller.
		 */
		void DeactivateController(const std::string_view& identifier);

		/**
		 * Run the client application.
		 */
		void RunClient();

		/**
		 * Get all the active controllers.
		 * 
		 * @return The controller map.
		 */
		std::unordered_map<std::string_view, std::shared_ptr<Controller>> GetActiveControllers();

		/**
		 * Get all the active controllers.
		 *
		 * @return The controller map.
		 */
		const std::unordered_map<std::string_view, std::shared_ptr<Controller>> GetActiveControllers() const;

	protected:
		/**
		 * Register an identifier.
		 * If a class is not registered, the client will not be able to use the controller.
		 *
		 * @param identifier The class identifier.
		 */
		void RegisterIdentifier(const std::string& identifier);

	protected:
		std::vector<std::string> mRegisteredClasses = {};
		std::unordered_map<std::string_view, std::shared_ptr<Controller>> mActiveControllers = {};
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

#ifdef FLINT_PLATFORM_WINDOWS
#define EXPORT_FUNCTION			__declspec(dllexport)

#endif

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
	EXPORT_FUNCTION ::Flint::ClientInterface* CreateClient()										\
	{																								\
		return new className(__VA_ARGS__);															\
	}																								\
	 																								\
	EXPORT_FUNCTION void DestroyClient(::Flint::ClientInterface* pClient)							\
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
