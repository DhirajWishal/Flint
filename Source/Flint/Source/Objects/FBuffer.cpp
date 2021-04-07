// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint\Objects\FBuffer.h"

#if defined(FLINT_BACKEND_VULKAN)
#include "VulkanBackend\VulkanBuffer.h"
typedef Flint::VulkanBackend::VulkanBuffer Buffer;

#elif defined(FLINT_BACKEND_DIRECT_X_12)

#elif defined(FLINT_BACKEND_WEB_GPU)

#else
error "Selected backend is invalid! Make sure that you select a proper supported backend!"

#endif // defined(FLINT_BACKEND_VULKAN)


namespace Flint
{
	FBuffer::FBuffer()
	{
		Construct<Buffer>();
	}

	FBuffer::FBuffer(Backend::BackendObject* pBackendObject)
	{
		pBackendObject = pBackendObject;
	}

	FBuffer::~FBuffer()
	{
		Destruct<Buffer>();
	}

	void FBuffer::Initialize(const FDevice& device, UI64 size, Backend::BufferUsage usage, Backend::MemoryProfile profile)
	{
		GetAs<Buffer>().Initialize(static_cast<Buffer::DeviceType*>(device.GetBackendObject()), size, usage, profile);
	}

	void FBuffer::Terminate()
	{
		GetAs<Buffer>().Terminate();
	}
	
	void* FBuffer::MapMemory(UI64 size, UI64 offset)
	{
		return GetAs<Buffer>().MapMemory(size, offset);
	}

	void FBuffer::FlushMemoryMapping()
	{
		GetAs<Buffer>().FlushMemoryMappings();
	}

	void FBuffer::UnmapMemory()
	{
		GetAs<Buffer>().UnmapMemory();
	}
	
	void FBuffer::CopyFrom(const FBuffer& other, UI64 size, UI64 srcOffset, UI64 dstOffset)
	{
		GetAs<Buffer>().CopyFrom(static_cast<Buffer&>(*other.GetBackendObject()), size, srcOffset, dstOffset);
	}
	
	FDevice FBuffer::GetDevice() const
	{
		return FDevice(GetAs<Buffer>().GetDevice());
	}
	
	UI64 FBuffer::GetSize() const
	{
		return GetAs<Buffer>().GetSize();
	}
}