// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "LowLevel/Backend.h"
#include "Core/Backend/Interface.h"
#include "Core/ErrorHandler/Logger.h"

#include "VulkanBackend/VulkanExporter.h"

namespace Flint
{
	namespace LowLevel
	{
		void SetupBackend(const BackendAPI& api)
		{
			switch (api)
			{
			case Flint::LowLevel::BackendAPI::VULKAN:
				VulkanBackend::ExportFunctions();
				break;
			default:
				FLINT_LOG_FATAL(TEXT("Invalid or Unsupported Backend API!"))
					break;
			}

			FLINT_ASSERT(Interface::IsInitialized(), TEXT("The backend functions are not (fully) exported!"))
		}
	}
}