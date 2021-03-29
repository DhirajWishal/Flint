// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include <type_traits>

namespace Flint
{
	namespace Backend
	{
		template<class Derived>
		class BackendObject {
			static_assert(std::is_same<BackendObject<Derived>, Derived>::value, "Type 'Derived' and object should not match!");
			static_assert(!std::is_base_of<BackendObject<Derived>, Derived>::value, "Type 'Derived' must be derived from the object!");

		public:
			using Derived = Derived;
			using UniquePointer = std::unique_ptr<Derived>;
			using SharedPointer = std::shared_ptr<Derived>;

		public:
			BackendObject() {}
			virtual ~BackendObject() {}

			Derived& GetDerived() { return *reinterpret_cast<Derived*>(this); }
		};
	}
}