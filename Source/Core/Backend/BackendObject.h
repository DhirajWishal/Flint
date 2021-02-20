// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Types/DataTypes.h"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Backend object.
		 * This is the base class for all the backend related objects.
		 */
		class BackendObject {
		public:
			BackendObject() {}
			virtual ~BackendObject() {}

			template<class Derived>
			Derived* Derive() { return dynamic_cast<Derived*>(this); }
		};
	}
}