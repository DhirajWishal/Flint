// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "VulkanBackend\VulkanExporter.h"

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

		Display* GetDisplay() const { return const_cast<Display*>(&mDisplay); }
		Device* GetDevice() const { return const_cast<Device*>(&mDevice); }

	protected:
		void InitializeInstance(BackendAPI api, bool enableValidation);
		void TerminateInstance();

		void InitializeDisplay(UI32 width, UI32 height, const char* pTitle);
		void TerminateDisplay();

		void InitializeDevice();
		void TerminateDevice();

	protected:
		Instance mInstance = {};
		Display mDisplay = {};
		Device mDevice = {};

		BackendAPI mAPI = BackendAPI::UNDEFINED;
	};
}