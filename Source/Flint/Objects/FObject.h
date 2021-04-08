// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core\Types\DataTypes.h"
#include "Core\Backend\BackendObject.h"

#include <memory>

namespace Flint
{
	/**
	 * Flint object.
	 * This object is the base class for all the objects in flint.
	 * 
	 * FObjects are proxy classes abstracting the underlying type and providing a better API.
	 */
	class FObject {
	public:
		FObject() {}
		FObject(const std::shared_ptr<Backend::BackendObject>& ptr) : pBackendObject(ptr) {}
		virtual ~FObject() {}

		/**
		 * Get the backend object pointer.
		 *
		 * @return The backend object pointer.
		 */
		std::shared_ptr<Backend::BackendObject> GetBackendObject() const { return pBackendObject; }

	protected:
		template<class Object>
		void Construct() { pBackendObject = std::make_shared<Object>(); }

		template<class Derived>
		Derived& GetAs() const { return static_cast<Derived&>(*pBackendObject); }

		std::shared_ptr<Backend::BackendObject> pBackendObject = {};
	};
}