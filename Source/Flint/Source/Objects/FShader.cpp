// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint\Objects\FShader.h"

#if defined(FLINT_BACKEND_VULKAN)
#include "VulkanBackend\VulkanShaderModule.h"
typedef Flint::VulkanBackend::VulkanShaderModule Module;

#elif defined(FLINT_BACKEND_DIRECT_X_12)

#elif defined(FLINT_BACKEND_WEB_GPU)

#else
error "Selected backend is invalid! Make sure that you select a proper supported backend!"

#endif // defined(FLINT_BACKEND_VULKAN)


namespace Flint
{
	FShader::FShader()
	{
		Construct<Module>();
	}

	FShader::~FShader()
	{
		Destruct<Module>();
	}

	void FShader::Initialize(const FDevice& device, const ShaderDigest& digest)
	{
		GetAs<Module>().Initialize(static_cast<Module::DeviceType*>(device.GetBackendObject()), digest);
	}

	void FShader::Terminate()
	{
		GetAs<Module>().Terminate();
	}
}