// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Engine/ClientInterface.hpp"

namespace Flint
{
	/**
	 * Editor controller view UI.
	 * Controller view displays all the controllers loaded by the client application.
	 */
	class ControllerView
	{
	public:
		ControllerView(const std::vector<std::string_view>& controllers);
		ControllerView(ClientInterface* pInterface);
		~ControllerView();
	};
}