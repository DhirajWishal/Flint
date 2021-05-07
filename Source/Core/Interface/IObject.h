// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

namespace Flint
{
	namespace Interface
	{
		class IObject
		{
		public:
			IObject() {}
			virtual ~IObject() {}

			virtual void Terminate() = 0;

			template<class Derived>
			Derived* Derive() { return dynamic_cast<Derived*>(this); }

			template<class Derived>
			const Derived* Derive() const { return dynamic_cast<Derived*>(this); }

			template<class Type>
			Type* GetAs() { return static_cast<Type*>(this); }

			template<class Type>
			const Type* GetAs() const { return static_cast<const Type*>(this); }
		};
	}
}