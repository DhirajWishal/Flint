// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint\Objects\FDevice.h"
#include "Flint\Objects\FScreenBoundRenderTarget.h"

#include "VulkanBackend\VulkanDevice.h"
typedef Flint::VulkanBackend::VulkanDevice Device;

namespace Flint
{
	constexpr const UI64 GetDeviceSize()
	{
		return sizeof(Device);
	}

	constexpr const UI64 GetDeviceAlignment()
	{
		return alignof(Device);
	}

	FDevice::FDevice()
	{
		Construct<Device>();
	}

	FDevice::~FDevice()
	{
		Destruct<Device>();
	}

	void FDevice::Initialize(const FInstance& instance)
	{
		GetAs<Device>().Initialize(static_cast<Device::InstanceType*>(instance.GetBackendObject()));
	}

	void FDevice::Terminate()
	{
		GetAs<Device>().Terminate();
	}
	
	FScreenBoundRenderTarget FDevice::CreateScreenBoundRenderTarget(const FDisplay& display, UI64 bufferCount)
	{
		FScreenBoundRenderTarget renderTarget = {};
		renderTarget.Initialize(*this, display, bufferCount);

		return renderTarget;
	}
}