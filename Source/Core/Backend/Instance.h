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
		class Instance : public BackendObject {
		public:
			Instance() {}
			virtual ~Instance() {}

			virtual void Initialize(bool enableValidation) = 0;
			virtual void Terminate() = 0;

			bool IsValidationEnabled() const { return bEnableValidation;  }

			FLINT_SET_NO_COPY_AND_MOVE(Instance)
		protected:
			bool bEnableValidation = true;
		};
	}
}