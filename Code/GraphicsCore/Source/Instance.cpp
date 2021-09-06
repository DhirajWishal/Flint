// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/Instance.hpp"

#include "VulkanBackend/VulkanInstance.hpp"

namespace Flint
{
	Instance::Instance(bool enableValidation)
		: mEnableValidation(enableValidation)
	{
		std::filesystem::path workingDirectory = std::filesystem::current_path();
		if (!std::filesystem::exists(workingDirectory.string() + "\\Flint"))
			std::filesystem::create_directory(workingDirectory.string() + "\\Flint");

		std::filesystem::path flintRuntimeDirectory = workingDirectory.string() + "\\Flint";
		if (!std::filesystem::exists(flintRuntimeDirectory.string() + "\\Cache"))
			std::filesystem::create_directory(flintRuntimeDirectory.string() + "\\Cache");

		if (!std::filesystem::exists(flintRuntimeDirectory.string() + "\\Shaders"))
			std::filesystem::create_directory(flintRuntimeDirectory.string() + "\\Shaders");
	}

	std::shared_ptr<Instance> CreateInstance(bool enableValidation)
	{
		return std::make_shared<VulkanBackend::VulkanInstance>(enableValidation);
	}
}