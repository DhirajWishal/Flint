// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DataType.hpp"

#include <stdexcept>

namespace Flint
{
	/**
	 * Flint backend error exception.
	 * This error is thrown if there was an error in the backend (in the backend API).
	 *
	 * This exception is a derived class of std::runtime_error.
	 */
	class backend_error final : public std::runtime_error
	{
	public:
		backend_error(const std::string& msg);
		backend_error(const char* msg);
	};
}

#ifndef FLINT_RELEASE
#	define FLINT_ASSERT(expression, condition, error)					if (expression != condition) error
#	define FLINT_ASSERT_DISCARDABLE(expression, condition, error)		if (expression != condition) error;

#else
#	define FLINT_ASSERT(expression, condition, error)					expression
#	define FLINT_ASSERT_DISCARDABLE(expression, condition, error)

#endif // !FLINT_RELEASE

namespace Flint
{
	/* Backend error constants. */

	constexpr char BackendErrorUnknown[] = "Unknown error!";
	constexpr char BackendErrorDeviceNotReady[] = "Device not ready!";
	constexpr char BackendErrorDeviceTimeout[] = "Device timeout!";
	constexpr char BackendErrorEventSet[] = "Event set!";
	constexpr char BackendErrorEventReset[] = "Event reset!";
	constexpr char BackendErrorIncompleteCommand[] = "Incomplete command!";
	constexpr char BackendErrorDeviceOutOfHostMemory[] = "Device out of host memory!";
	constexpr char BackendErrorDeviceOutOfDeviceMemory[] = "Device out of device memory!";
	constexpr char BackendErrorInitializationFailed[] = "Initialization failed!";
	constexpr char BackendErrorDeviceLost[] = "Device lost!";
	constexpr char BackendErrorMemoryMappingFailed[] = "Memory mapping failed!";
	constexpr char BackendErrorDeviceFeatureNotPresent[] = "Device feature not present!";
	constexpr char BackendErrorIncompatibleDriver[] = "Incompatible driver!";
	constexpr char BackendErrorTooManyObjects[] = "Too many objects!";
	constexpr char BackendErrorFormatNotSupported[] = "Format not supported!";
	constexpr char BackendErrorFragmentedPool[] = "Fragmented pool!";
	constexpr char BackendErrorOutOfPoolMemory[] = "Out of pool memory!";
	constexpr char BackendErrorInvalidHandle[] = "Invalid handle!";
	constexpr char BackendErrorInternalFragmentation[] = "Internal fragmentation!";
	constexpr char BackendErrorDisplayLost[] = "Display lost!";
	constexpr char BackendErrorDisplayInUse[] = "Display in use!";
	constexpr char BackendErrorRenderTargetSuboptimal[] = "Render target suboptimal!";
	constexpr char BackendErrorRenderTargetOutOfDate[] = "Render target out of date!";
	constexpr char BackendErrorIncompatibleDisplay[] = "Incompatible display!";
	constexpr char BackendErrorValidationFailed[] = "Validation failed!";
	constexpr char BackendErrorInvalidShader[] = "Invalid shader!";
	constexpr char BackendErrorOperationNotPermitted[] = "Operation not permitted!";
	constexpr char BackendErrorFullScreenModeLost[] = "Full screen mode lost!";
	constexpr char BackendErrorDeviceThreadIdle[] = "Device thread idle!";
	constexpr char BackendErrorDeviceThreadDone[] = "Device thread done!";
	constexpr char BackendErrorOperationDeferred[] = "Operation deferred!";
	constexpr char BackendErrorOperationNotDeferred[] = "Operation not deferred!";
	constexpr char BackendErrorPipelineCompilationRequired[] = "Pipeline compilation required!";
	constexpr char BackendErrorVulkanLayerNotPresent[] = "Vulkan layer not present!";
	constexpr char BackendErrorVulkanExtensionNotPresent[] = "Vulkan extension not present!";
	constexpr char BackendErrorVulkanInvalidOpaqueCaptureAddress[] = "Vulkan invalid opaque capture address!";
}