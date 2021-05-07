// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Core\Backend\Instance.h"
#include "Core\Types\DataTypes.h"

#include <memory>

namespace Flint
{
	enum class InstanceAPI : UI8 {
		VULKAN, WEB_GPU, DIRECT_X_12	
	};

	/**
	 * Flint instance object.
	 * Instance objects hold information about a single graphics backend API instance.
	 */
	class FInstance
	{
	public:
		FInstance() = default;
		FInstance(InstanceAPI api, bool enableValidation);

		void Terminate();

		InstanceAPI GetInstanceAPI() const { return mAPI; }
		bool IsValidationEnabled() const { return pInstance->IsValidationEnabled(); }

	private:
		std::unique_ptr<Backend::Instance> pInstance = nullptr;
		InstanceAPI mAPI = InstanceAPI::VULKAN;
	};
}