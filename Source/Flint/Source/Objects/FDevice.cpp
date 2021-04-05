// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint\Objects\FDevice.h"
#include "Flint\Objects\FScreenBoundRenderTarget.h"
#include "Flint\Objects\FShader.h"
#include "Flint\Objects\FBuffer.h"

#if defined(FLINT_BACKEND_VULKAN)
#include "VulkanBackend\VulkanDevice.h"
typedef Flint::VulkanBackend::VulkanDevice Device;

#endif // defined(FLINT_BACKEND_VULKAN)


namespace Flint
{
	FDevice::FDevice()
	{
		Construct<Device>();
	}

	FDevice::FDevice(Backend::BackendObject* pBackendObject)
	{
		std::copy(reinterpret_cast<BYTE*>(pBackendObject), reinterpret_cast<BYTE*>(pBackendObject) + FLINT_DEVICE_BACKEND_SIZE, pBuffer);
		this->pBackendObject = reinterpret_cast<Backend::BackendObject*>(pBuffer);
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