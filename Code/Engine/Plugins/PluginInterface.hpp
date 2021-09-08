// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/FObject.hpp"

namespace Flint
{
	namespace Plugins
	{
		/**
		 * Plugin permissions enum.
		 */
		enum class PluginPermissions : UI32 {
			RenderCapabilities = BitShiftLeft(0),
		};

		constexpr PluginPermissions operator|(const PluginPermissions& lhs, const PluginPermissions& rhs) { return static_cast<PluginPermissions>(static_cast<UI32>(lhs) | static_cast<UI32>(rhs)); }
		constexpr PluginPermissions operator&(const PluginPermissions& lhs, const PluginPermissions& rhs) { return static_cast<PluginPermissions>(static_cast<UI32>(lhs) & static_cast<UI32>(rhs)); }

		/**
		 * Flint plugin interface object.
		 * All the plugins that are required to be attached to the engine should be inherited from this object.
		 */
		class PluginInterface : public FObject
		{
		public:
			PluginInterface(const std::string& name) : mPluginName(name) {}

			/**
			 * Get the required permissions for the plugin.
			 * 
			 * @return The plugin permissions.
			 */
			virtual PluginPermissions GetPermissions() const = 0;

		protected:
			std::string mPluginName = "";
		};

		typedef PluginInterface* (*TCreatePluginFunction)();
		typedef void (*TDestroyPluginFunction)(PluginInterface*);
	}
}

#ifdef FLINT_PLATFORM_WINDOWS
#define EXPORT_SHARED_LIB	__declspec(dllexport)

#endif

/**
 * Flint export plugin macro.
 * All the plugins must be exported using this macro if the engine is to attach the plugin. If not the plugin will not be visible to the engine at runtime.
 */
#define FLINT_EXPORT_PLUGIN(plugin, ...)																	\
extern "C"																									\
{																											\
	EXPORT_SHARED_LIB ::Flint::Plugins::PluginInterface* CreatePlugin()										\
	{																										\
		return new plugin(__VA_ARGS__);																		\
	}																										\
																											\
	EXPORT_SHARED_LIB void DestroyPlugin(::Flint::Plugins::PluginInterface* pPlugin)						\
	{																										\
		delete pPlugin;																						\
	}																										\
}