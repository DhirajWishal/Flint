// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "PluginInterface.hpp"

namespace Flint
{
	namespace Plugins
	{
		/**
		 * Flint plugin handle object.
		 * This object handles information about a single plugin.
		 */
		class PluginHandle
		{
		public:
			PluginHandle(const std::filesystem::path& pluginPath) {}

			/**
			 * Check if the plugin is enabled.
			 * 
			 * @return The boolean state.
			 */
			const bool IsEnabled() const { return bIsEnabled; }

			/**
			 * Check if the plugin contains the permissions.
			 * 
			 * @return The boolean value.
			 */
			template<PluginPermissions Permission>
			const bool ContainPermissions() const { return (Permission & pPlugin->GetPermissions()) == Permission; }

		private:
			std::filesystem::path mPluginPath = "";
			PluginInterface* pPlugin = nullptr;

			bool bIsEnabled = true;
		};
	}
}