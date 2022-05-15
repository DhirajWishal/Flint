// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanInstance.hpp"
#include "VulkanBackend/VulkanEngine.hpp"

int main()
{
	auto instance = Flint::VulkanBackend::VulkanInstance("Sandbox", 1, true);
	auto engine = Flint::VulkanBackend::VulkanEngine(instance);

	return 0;
}