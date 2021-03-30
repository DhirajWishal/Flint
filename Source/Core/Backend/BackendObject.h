// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <type_traits>

namespace Flint
{
	namespace Backend
	{
		class BackendObject {
		public:
			BackendObject() {}
			virtual ~BackendObject() {}

			template<class Derived>
			Derived* Derive() { return dynamic_cast<Derived*>(this); }
		};
	}
}