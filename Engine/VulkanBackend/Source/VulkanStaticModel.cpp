// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanStaticModel.hpp"
#include "VulkanBackend/VulkanMacros.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanStaticModel::VulkanStaticModel(VulkanDevice& device, std::filesystem::path&& assetFile)
			: StaticModel(device, std::move(assetFile))
			, m_VertexStorage(device)
		{
			// Make sure to set the object as valid.
			validate();
		}

		VulkanStaticModel::~VulkanStaticModel()
		{
			FLINT_TERMINATE_IF_VALID;
		}

		void VulkanStaticModel::terminate()
		{
			invalidate();
		}
	}
}