// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanEngine.hpp"

#ifdef FLINT_DEBUG
constexpr auto Validation = true;

#else
constexpr auto Validation = false;

#endif

int main()
{
	auto instance = Flint::VulkanBackend::VulkanInstance("Sandbox", 1, Validation);

	// Note that the scope is here because the engine needs to be destructed BEFORE destroying the instance.
	{
		auto pEngine = instance.createEngine();
	}

	return 0;
}