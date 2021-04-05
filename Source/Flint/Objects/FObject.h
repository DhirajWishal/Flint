// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core\Types\DataTypes.h"
#include "Core\Backend\BackendObject.h"

namespace Flint
{
	/**
	 * Flint object.
	 * This object is the base class for all the objects in flint.
	 *
	 * Since the backend API is decided at compile time, the objects can be directly compiled without the need of polymorphism; thus improving performance. But still allocating the object somewhere
	 * else and accessing it using a pointer might not be the most cache friendly approach. So what flint does is, it allocates a byte buffer with the alignment of the object, followed by the size
	 * and the actual object's data will be stored in that buffer and accesses via a dereferenced parent pointer. This opts for better performance by data locality and with the help of stack allocations.
	 *
	 * @tparam Size: The size of the backend object.
	 * @tparam Alignment: The alignment of the backend object.
	 */
	template<UI64 Size, UI64 Alignment = FLINT_DEFAULT_ALIGNMENT>
	class FObject {
	public:
		FObject() {}
		virtual ~FObject() {}

		/**
		 * Get the backend object pointer.
		 *
		 * @return The backend object pointer.
		 */
		Backend::BackendObject* GetBackendObject() const { return pBackendObject; }

	protected:
		template<class Object>
		void Construct() { pBackendObject = new (pBuffer) Object(); }

		template<class Derived>
		Derived& GetAs() const { return static_cast<Derived&>(*pBackendObject); }

		template<class Derived>
		void Destruct() { GetAs<Derived>().~Derived(); }

		void ResetBuffer() { std::fill(pBuffer, pBuffer + Size, 0); }

		alignas(Alignment) BYTE pBuffer[Size] = {};
		Backend::BackendObject* pBackendObject = nullptr;
	};
}