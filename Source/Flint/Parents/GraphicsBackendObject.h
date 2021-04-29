// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core\Backend\FDevice.h"
#include "Core\Backend\FDisplay.h"

namespace Flint
{
	enum class BackendAPI : UI8 {
		UNDEFINED,
		VULKAN,
		DIRECT_X_12,
		WEB_GPU
	};

	/**
	 * Graphics backend object.
	 * This object contains the basic instance, display and device which the engine will use.
	 */
	class GraphicsBackendObject {
	public:
		GraphicsBackendObject() {}

		std::shared_ptr<FDisplay> GetDisplay() const { return pDisplay; }
		std::shared_ptr<FDevice> GetDevice() const { return pDevice; }

	protected:
		void InitializeInstance(BackendAPI api, bool enableValidation);
		void TerminateInstance();

		void InitializeDisplay(UI32 width, UI32 height, const char* pTitle);
		void TerminateDisplay();

		void InitializeDevice();
		void TerminateDevice();

	protected:
		std::shared_ptr<FInstance> pInstance = {};
		std::shared_ptr<FDisplay> pDisplay = {};
		std::shared_ptr<FDevice> pDevice = {};

		BackendAPI mAPI = BackendAPI::UNDEFINED;
	};
}