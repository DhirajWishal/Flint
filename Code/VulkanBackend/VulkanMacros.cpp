// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include <volk.h>
export module Flint.VulkanBackend.VulkanMacros;
import Flint.Core.Error;

export namespace Flint
{
	namespace VulkanBackend
	{
		namespace Utilities
		{
			inline void CheckResult(VkResult result)
			{
				switch (result)
				{
				case VkResult::VK_SUCCESS:											return;
				case VkResult::VK_NOT_READY:										throw backend_error(BackendErrorDeviceNotReady);
				case VkResult::VK_TIMEOUT:											throw backend_error(BackendErrorDeviceTimeout);
				case VkResult::VK_EVENT_SET:										throw backend_error(BackendErrorEventSet);
				case VkResult::VK_EVENT_RESET:										throw backend_error(BackendErrorEventReset);
				case VkResult::VK_INCOMPLETE:										throw backend_error(BackendErrorIncompleteCommand);
				case VkResult::VK_ERROR_OUT_OF_HOST_MEMORY:							throw backend_error(BackendErrorDeviceOutOfHostMemory);
				case VkResult::VK_ERROR_OUT_OF_DEVICE_MEMORY:						throw backend_error(BackendErrorDeviceOutOfDeviceMemory);
				case VkResult::VK_ERROR_INITIALIZATION_FAILED:						throw backend_error(BackendErrorInitializationFailed);
				case VkResult::VK_ERROR_DEVICE_LOST:								throw backend_error(BackendErrorDeviceLost);
				case VkResult::VK_ERROR_MEMORY_MAP_FAILED:							throw backend_error(BackendErrorMemoryMappingFailed);
				case VkResult::VK_ERROR_LAYER_NOT_PRESENT:							throw backend_error(BackendErrorVulkanLayerNotPresent);
				case VkResult::VK_ERROR_EXTENSION_NOT_PRESENT:						throw backend_error(BackendErrorVulkanExtensionNotPresent);
				case VkResult::VK_ERROR_FEATURE_NOT_PRESENT:						throw backend_error(BackendErrorDeviceFeatureNotPresent);
				case VkResult::VK_ERROR_INCOMPATIBLE_DRIVER:						throw backend_error(BackendErrorIncompatibleDriver);
				case VkResult::VK_ERROR_TOO_MANY_OBJECTS:							throw backend_error(BackendErrorTooManyObjects);
				case VkResult::VK_ERROR_FORMAT_NOT_SUPPORTED:						throw backend_error(BackendErrorFormatNotSupported);
				case VkResult::VK_ERROR_FRAGMENTED_POOL:							throw backend_error(BackendErrorFragmentedPool);
				case VkResult::VK_ERROR_OUT_OF_POOL_MEMORY:							throw backend_error(BackendErrorOutOfPoolMemory);
				case VkResult::VK_ERROR_INVALID_EXTERNAL_HANDLE:					throw backend_error(BackendErrorInvalidHandle);
				case VkResult::VK_ERROR_FRAGMENTATION:								throw backend_error(BackendErrorInternalFragmentation);
				case VkResult::VK_ERROR_INVALID_OPAQUE_CAPTURE_ADDRESS:				throw backend_error(BackendErrorVulkanInvalidOpaqueCaptureAddress);
				case VkResult::VK_ERROR_SURFACE_LOST_KHR:							throw backend_error(BackendErrorDisplayLost);
				case VkResult::VK_ERROR_NATIVE_WINDOW_IN_USE_KHR:					throw backend_error(BackendErrorDisplayInUse);
				case VkResult::VK_SUBOPTIMAL_KHR:									throw backend_error(BackendErrorRenderTargetSuboptimal);
				case VkResult::VK_ERROR_OUT_OF_DATE_KHR:							throw backend_error(BackendErrorRenderTargetOutOfDate);
				case VkResult::VK_ERROR_INCOMPATIBLE_DISPLAY_KHR:					throw backend_error(BackendErrorIncompatibleDisplay);
				case VkResult::VK_ERROR_VALIDATION_FAILED_EXT:						throw backend_error(BackendErrorValidationFailed);
				case VkResult::VK_ERROR_INVALID_SHADER_NV:							throw backend_error(BackendErrorInvalidShader);
				case VkResult::VK_ERROR_NOT_PERMITTED_EXT:							throw backend_error(BackendErrorOperationNotPermitted);
				case VkResult::VK_ERROR_FULL_SCREEN_EXCLUSIVE_MODE_LOST_EXT:		throw backend_error(BackendErrorFullScreenModeLost);
				case VkResult::VK_THREAD_IDLE_KHR:									throw backend_error(BackendErrorDeviceThreadIdle);
				case VkResult::VK_THREAD_DONE_KHR:									throw backend_error(BackendErrorDeviceThreadDone);
				case VkResult::VK_OPERATION_DEFERRED_KHR:							throw backend_error(BackendErrorOperationDeferred);
				case VkResult::VK_OPERATION_NOT_DEFERRED_KHR:						throw backend_error(BackendErrorOperationNotDeferred);
				case VkResult::VK_PIPELINE_COMPILE_REQUIRED_EXT:					throw backend_error(BackendErrorPipelineCompilationRequired);
				default:															throw backend_error(BackendErrorUnknown);
				}
			}
		}

		inline VkBool32 GetVkBool(const bool value) { return value ? VK_TRUE : VK_FALSE; }
	}
}
