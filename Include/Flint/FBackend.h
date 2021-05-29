// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "FBufferManager.h"
#include "FRenderTargetManager.h"

namespace Flint
{
	/**
	 * Flint backend object.
	 * This object holds the interface between the client application and the backend services.
	 */
	class FLINT_API FBackend :
		public FDeviceManager,
		public FDisplayManager,
		public FBufferManager,
		public FRenderTargetManager
	{
	public:
		FBackend(bool enableValidation) : bEnableValidation(enableValidation) {}

		/**
		 * Terminate the backend.
		 */
		virtual void Terminate() = 0;

	public:
		/**
		 * Check if validation is enabled.
		 *
		 * @return True or false stating if enabled or not respectively.
		 */
		bool IsValidationEnabled() const { return bEnableValidation; }

	protected:
		bool bEnableValidation = true;
	};

	/**
	 * Backend API enum.
	 * This enum contains all the supported backend APIs.
	 */
	enum class BackendAPI : UI8 {
		VULKAN
	};

	/**
	 * Create a new backend object.
	 *
	 * @param api: The backend API.
	 * @param enableValidation: Whether or not to enable backend validation.
	 * @return The created backend pointer.
	 */
	FBackend* CreateBackend(BackendAPI api, bool enableValidation);
}