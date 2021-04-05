// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Flint\Objects\FDisplay.h"

#if defined(FLINT_BACKEND_VULKAN)
#include "VulkanBackend\VulkanDisplay.h"
typedef Flint::VulkanBackend::VulkanDisplay Display;

#endif // defined(FLINT_BACKEND_VULKAN)


namespace Flint
{
	FDisplay::FDisplay()
	{
		Construct<Display>();
	}

	FDisplay::~FDisplay()
	{
		Destruct<Display>();
	}

	void FDisplay::Initialize(const FInstance& instance, const Vector2 extent, const char* pTitle)
	{
		GetAs<Display>().Initialize(static_cast<Display::InstanceType*>(instance.GetBackendObject()), extent, pTitle);
	}

	void FDisplay::Terminate()
	{
		GetAs<Display>().Terminate();
	}

	void FDisplay::Update()
	{
		GetAs<Display>().Update();
	}

	Inputs::InputCenter* FDisplay::GetInputCenter() const
	{
		return GetAs<Display>().GetInputCenter();
	}
	
	Vector2 FDisplay::GetExtent() const
	{
		return GetAs<Display>().GetExtent();
	}
}