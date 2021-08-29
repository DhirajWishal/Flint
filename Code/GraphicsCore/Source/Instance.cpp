// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "GraphicsCore/Instance.hpp"

#include "VulkanBackend/VulkanInstance.hpp"

namespace Flint
{
	Instance::Instance(bool enableValidation)
		: mEnableValidation(enableValidation)
	{
		boost::filesystem::path workingDirectory = boost::filesystem::current_path();
		if (!boost::filesystem::exists(workingDirectory.string() + "\\Flint"))
			boost::filesystem::create_directory(workingDirectory.string() + "\\Flint");

		boost::filesystem::path flintRuntimeDirectory = workingDirectory.string() + "\\Flint";
		if (!boost::filesystem::exists(flintRuntimeDirectory.string() + "\\Cache"))
			boost::filesystem::create_directory(flintRuntimeDirectory.string() + "\\Cache");

		if (!boost::filesystem::exists(flintRuntimeDirectory.string() + "\\Shaders"))
			boost::filesystem::create_directory(flintRuntimeDirectory.string() + "\\Shaders");
	}

	boost::shared_ptr<Instance> CreateInstance(bool enableValidation)
	{
		return boost::make_shared<VulkanBackend::VulkanInstance>(enableValidation);
	}
}