// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <type_traits>
#include "Core\Macros\Global.h"

namespace Flint
{
	namespace Backend
	{
		class FLINT_ALIGN BackendObject {
		public:
			BackendObject() {}
			virtual ~BackendObject() {}

			template<class Derived>
			Derived* Derive() { return dynamic_cast<Derived*>(this); }
		};
	}
}