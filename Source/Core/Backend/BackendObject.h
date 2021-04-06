// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <type_traits>
#include "Core\Macros\Global.h"

namespace Flint
{
	namespace Backend
	{
		/**
		 * Backend object.
		 * This object is the base class for all the backend objects in the library.
		 */
		class BackendObject {
		public:
			BackendObject() {}
			virtual ~BackendObject() {}

			/**
			 * Cast the object to a derived type.
			 * 
			 * @tparam Derived: The derived type to cast to.
			 */
			template<class Derived>
			Derived* Derive() { return dynamic_cast<Derived*>(this); }
		};
	}
}