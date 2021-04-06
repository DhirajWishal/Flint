// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint\Objects\FInstance.h"
#include "Flint\Objects\FDisplay.h"
#include "Flint\Objects\FDevice.h"

#if defined(FLINT_BACKEND_VULKAN)
#include "VulkanBackend\VulkanInstance.h"
typedef Flint::VulkanBackend::VulkanInstance Instance;

#endif // defined(FLINT_BACKEND_VULKAN)


namespace Flint
{
	FInstance::FInstance()
	{
		Construct<Instance>();
	}

	FInstance::~FInstance()
	{
		Destruct<Instance>();
	}

	void FInstance::Initialize(bool enableValidation)
	{
		GetAs<Instance>().Initialize(enableValidation);
	}

	void FInstance::Terminate()
	{
		GetAs<Instance>().Terminate();
	}
	
	FDisplay FInstance::CreateDisplay(const Vector2 extent, const char* pTitle)
	{
		FDisplay display = {};
		display.Initialize(*this, extent, pTitle);

		return display;
	}
	
	FDevice FInstance::CreateDevice()
	{
		FDevice device = {};
		device.Initialize(*this);

		return device;
	}
}