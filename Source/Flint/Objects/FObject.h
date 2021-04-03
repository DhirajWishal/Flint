// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core\Types\DataTypes.h"

namespace Flint
{
	template<UI64 Size, UI64 Alignment>
	class FObject {
	public:
		FObject() {}
		virtual ~FObject() {}

		Backend::BackendObject* GetBackendObject() const { return pBackendObject; }

	protected:
		template<class Object>
		void Construct() { pBackendObject = new (pBuffer) Object(); }

		template<class Derived>
		void Destruct() { GetAs<Derived>().~Derived(); }

		template<class Derived>
		Derived& GetAs() const { return static_cast<Derived&>(*pBackendObject); }

		alignas(Alignment) BYTE pBuffer[Size] = {};
		Backend::BackendObject* pBackendObject = nullptr;
	};
}