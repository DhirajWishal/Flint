// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core/Backend/Instance.h"

namespace Flint
{
	enum class BackenAPI : UI8 {
		UNDEFINED,
		VULKAN,
		DIRECT_X_12,
		WEB_GPU
	};

	/**
	 * Flint Engine object.
	 * This is the main object which is responsible of all flint activities.
	 */
	class Engine {
	public:
		Engine() {}
		~Engine() {}

		/**
		 * Initialize the engine.
		 *
		 * @param api: The backend API.
		 * @param enableValidation: Whether or not to enable backend validation.
		 */
		void Initialize(BackenAPI api, bool enableValidation);

		/**
		 * Terminate the engine.
		 */
		void Terminate();

		/**
		 * Switch from the current backend API to a new API.
		 *
		 * @param newAPI: The new api to be set.
		 */
		void SwitchAPI(BackenAPI newAPI);

		/**
		 * Get the backend API which is currently active.
		 *
		 * @return The backend API type.
		 */
		BackenAPI GetBackendAPI() const { return mAPI; }

	private:
		Backend::Instance* pInstance = nullptr;

		BackenAPI mAPI = BackenAPI::UNDEFINED;
	};
}