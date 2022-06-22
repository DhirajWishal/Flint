// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanStaticModel.hpp"
#include "VulkanBackend/VulkanMacros.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanStaticModel::VulkanStaticModel(const std::shared_ptr<VulkanDevice>& pDevice, std::filesystem::path&& assetFile)
			: StaticModel(pDevice, std::move(assetFile))
			, m_VertexStorage(pDevice)
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