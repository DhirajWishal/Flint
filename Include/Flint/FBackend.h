// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FBufferManager.h"

namespace Flint
{
	/**
	 * Flint backend object.
	 * This object holds the interface between the client application and the backend services.
	 */
	class FLINT_API FBackend :
		public FDeviceManager,
		public FDisplayManager,
		public FBufferManager
	{
	public:
		FBackend(bool enableValidation) : bEnableValidation(enableValidation) {}

		/**
		 * Terminate the backend.
		 */
		virtual void Terminate() = 0;

	protected:
		bool bEnableValidation = true;
	};
}