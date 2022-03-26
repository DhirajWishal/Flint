// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "GeometryStoreManager.hpp"

namespace Flint
{
	/**
	 * Flint engine object.
	 * All the client application uses this object to perform graphical operations.
	 * 
	 * The engine is a low level framework and is not thread safe. The client application can push the engine to another thread and let it work there but parts of the engine to work on
	 * a different thread is not advised.
	 */
	class Engine final : public GeometryStoreManager
	{
	public:
		Engine() {}
	};
}
