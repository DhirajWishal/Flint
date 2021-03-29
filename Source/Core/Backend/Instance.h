// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "BackendObject.h"
#include <memory>

#include "Core\Types\DataTypes.h"
#include "Core\Macros\Global.h"

#define FLINT_BACKEND_OBJECT_ASSERT(base, derived, msg) static_assert(!std::is_base_of<base, derived>::value, msg);

namespace Flint
{
	namespace Backend
	{
		template<class Derived>
		class Instance : public BackendObject<Derived> {
		public:
			Instance() {}
			virtual ~Instance() {}

			void Initialize(bool enableValidation) { mEnableValidation = enableValidation; GetDerived().mInitialize(); }
			void Terminate() { GetDerived().mTerminate(); }

			bool IsValidationEnabled() const { return mEnableValidation;  }

			FLINT_SET_NO_COPY_AND_MOVE(Instance)
		protected:
			virtual void mInitialize() = 0;
			virtual void mTerminate() = 0;

			bool mEnableValidation = true;
		};
	}
}