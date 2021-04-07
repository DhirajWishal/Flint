// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint\Objects\FDevice.h"
#include "Flint\Objects\FScreenBoundRenderTarget.h"
#include "Flint\Objects\FShader.h"
#include "Flint\Objects\FBuffer.h"

#if defined(FLINT_BACKEND_VULKAN)
#include "VulkanBackend\VulkanDevice.h"
typedef Flint::VulkanBackend::VulkanDevice Device;

#elif defined(FLINT_BACKEND_DIRECT_X_12)

#elif defined(FLINT_BACKEND_WEB_GPU)

#else
error "Selected backend is invalid! Make sure that you select a proper supported backend!"

#endif // defined(FLINT_BACKEND_VULKAN)


namespace Flint
{
	FDevice::FDevice()
	{
		Construct<Device>();
		bShouldDelete = true;
	}

	FDevice::FDevice(Backend::BackendObject* pBackendObject)
	{
		this->pBackendObject = pBackendObject;
	}

	FDevice::~FDevice()
	{
		if (bShouldDelete)
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

	FShader FDevice::CreateShader(const ShaderDigest& digest)
	{
		FShader shader = {};
		shader.Initialize(*this, digest);

		return shader;
	}

	FBuffer FDevice::CreateBuffer(UI64 size, Backend::BufferUsage usage, Backend::MemoryProfile profile)
	{
		FBuffer buffer = {};
		buffer.Initialize(*this, size, usage, profile);

		return buffer;
	}
}