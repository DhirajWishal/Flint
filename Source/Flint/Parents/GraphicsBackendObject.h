// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Flint\Objects\FDevice.h"
#include "Flint\Objects\FDisplay.h"

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

		FDisplay* GetDisplay() const { return const_cast<FDisplay*>(&mDisplay); }
		FDevice* GetDevice() const { return const_cast<FDevice*>(&mDevice); }

	protected:
		void InitializeInstance(BackendAPI api, bool enableValidation);
		void TerminateInstance();

		void InitializeDisplay(UI32 width, UI32 height, const char* pTitle);
		void TerminateDisplay();

		void InitializeDevice();
		void TerminateDevice();

	protected:
		FInstance mInstance = {};
		FDisplay mDisplay = {};
		FDevice mDevice = {};

		BackendAPI mAPI = BackendAPI::UNDEFINED;
	};
}