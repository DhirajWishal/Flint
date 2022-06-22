// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "VulkanBackend/VulkanVertexStorage.hpp"

namespace Flint
{
	namespace VulkanBackend
	{
		VulkanVertexStorage::VulkanVertexStorage(const std::shared_ptr<VulkanDevice>& pDevice)
			: VertexStorage(pDevice)
		{
			validate();
		}

		VulkanVertexStorage::~VulkanVertexStorage()
		{
			FLINT_TERMINATE_IF_VALID;
		}

		void VulkanVertexStorage::terminate()
		{
			invalidate();
		}
	}
}