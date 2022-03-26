// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Plugins/PluginHandle.hpp"

namespace Flint
{
	/**
	 * Flint plugin manager object.
	 * This object handles all the aspects of a plugin.
	 */
	class PluginManager
	{
	public:
		PluginManager() = default;

	protected:
		std::vector<Plugins::PluginHandle> mPluginHandles = {};
	};
}