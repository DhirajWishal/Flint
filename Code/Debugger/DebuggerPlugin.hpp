// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Engine/Plugins/PluginInterface.hpp"

namespace Flint
{
	namespace Debugger
	{
		class DebuggerPlugin final : public Plugins::PluginInterface
		{
		public:
			DebuggerPlugin() : PluginInterface("Debugger Plugin") {}

			virtual Plugins::PluginPermissions GetPermissions() const override final { return Plugins::PluginPermissions::RenderCapabilities; }
		};
	}
}