// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/Interface/Handles.h"

namespace Flint
{
	namespace LowLevel
	{
		/**
		 * Backend Instance object.
		 */
		class Instance {
		public:
			Instance() {}
			~Instance() {}

			/**
			 * Initialize the backend instance.
			 *
			 * @param enableValidation: Whether or not to enable backend validation. We recommend not to enable this on
			 * the release mode as itll use up some performance.
			 */
			void Initialize(bool enableValidation);

			/**
			 * Terminate the backend instance.
			 */
			void Terminate();

		public:
			Interface::InstanceHandle GetHandle() const { return mHandle; }

		private:
			Interface::InstanceHandle mHandle = FLINT_NULL_HANDLE;
		};
	}
}