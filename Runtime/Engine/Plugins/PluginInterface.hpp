// Copyright 2021-2022 Dhiraj Wishal
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
		enum class PluginPermissions : uint32_t
		{
			RenderCapabilities = BitShiftLeft(0),
		};

		constexpr PluginPermissions operator|(const PluginPermissions& lhs, const PluginPermissions& rhs) { return static_cast<PluginPermissions>(static_cast<uint32_t>(lhs) | static_cast<uint32_t>(rhs)); }
		constexpr PluginPermissions operator&(const PluginPermissions& lhs, const PluginPermissions& rhs) { return static_cast<PluginPermissions>(static_cast<uint32_t>(lhs) & static_cast<uint32_t>(rhs)); }

		/**
		 * Flint plugin interface object.
		 * All the plug ins that are required to be attached to the engine should be inherited from this object.
		 */
		class PluginInterface : public FObject
		{
		public:
			PluginInterface(const std::string& name) : m_PluginName(name) {}

			/**
			 * Get the required permissions for the plugin.
			 *
			 * @return The plugin permissions.
			 */
			virtual PluginPermissions GetPermissions() const = 0;

		protected:
			std::string m_PluginName = "";
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
 * All the plug ins must be exported using this macro if the engine is to attach the plugin. If not the plugin will not be visible to the engine at runtime.
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