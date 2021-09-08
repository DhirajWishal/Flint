// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Engine/ComponentRegistry.hpp"

namespace Flint
{
	/**
	 * Flint global component view.
	 */
	class GlobalComponentView
	{
	public:
		GlobalComponentView(TGlobalComponentMap& componentMap);
		~GlobalComponentView();
	};
}
