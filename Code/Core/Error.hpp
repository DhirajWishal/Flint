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

	/**
	 * Flint extended error exception.
	 * This exception behaves as a normal runtime error but with extended support to display other information like file name, function signature and line number.
	 *
	 * This exception is a derived class of std::runtime_error.
	 */
	class runtime_error_extended final : public std::runtime_error
	{
	public:
		runtime_error_extended(const std::string& msg, const char* pErrorLocation);
		runtime_error_extended(const char* msg, const char* pErrorLocation);

		/**
		 * Get the location info from which the error is thrown.
		 *
		 * @return The function signature.
		 */
		const char* GetErrorLocationInfo() const { return pErrorLocationInfo; }

	private:
		const char* pErrorLocationInfo = nullptr;
	};
}

#ifndef FLINT_RELEASE
#	define FLINT_THROW_RUNTIME_ERROR(...)								throw ::Flint::runtime_error_extended(__VA_ARGS__, "\nFile:\t\t" __FILE__ "\nFunction:\t" __FUNCSIG__ "\nLine:\t\t" TO_STRING(__LINE__))
#	define FLINT_ASSERT(expression, condition, error)					if (expression != condition) error
#	define FLINT_ASSERT_DISCARDABLE(expression, condition, error)		if (expression != condition) error;

#else
#	define FLINT_THROW_RUNTIME_ERROR									throw std::runtime_error
#	define FLINT_ASSERT(expression, condition, error)					expression
#	define FLINT_ASSERT_DISCARDABLE(expression, condition, error)

#endif // !FLINT_RELEASE

namespace Flint
{
	/* Backend error constants. */

	constexpr const char BackendErrorUnknown[] = "Unknown error!";
	constexpr const char BackendErrorDeviceNotReady[] = "Device not ready!";
	constexpr const char BackendErrorDeviceTimeout[] = "Device timeout!";
	constexpr const char BackendErrorEventSet[] = "Event set!";
	constexpr const char BackendErrorEventReset[] = "Event reset!";
	constexpr const char BackendErrorIncompleteCommand[] = "Incomplete command!";
	constexpr const char BackendErrorDeviceOutOfHostMemory[] = "Device out of host memory!";
	constexpr const char BackendErrorDeviceOutOfDeviceMemory[] = "Device out of device memory!";
	constexpr const char BackendErrorInitializationFailed[] = "Initialization failed!";
	constexpr const char BackendErrorDeviceLost[] = "Device lost!";
	constexpr const char BackendErrorMemoryMappingFailed[] = "Memory mapping failed!";
	constexpr const char BackendErrorDeviceFeatureNotPresent[] = "Device feature not present!";
	constexpr const char BackendErrorIncompatibleDriver[] = "Incompatible driver!";
	constexpr const char BackendErrorTooManyObjects[] = "Too many objects!";
	constexpr const char BackendErrorFormatNotSupported[] = "Format not supported!";
	constexpr const char BackendErrorFragmentedPool[] = "Fragmented pool!";
	constexpr const char BackendErrorOutOfPoolMemory[] = "Out of pool memory!";
	constexpr const char BackendErrorInvalidHandle[] = "Invalid handle!";
	constexpr const char BackendErrorInternalFragmentation[] = "Internal fragmentation!";
	constexpr const char BackendErrorDisplayLost[] = "Display lost!";
	constexpr const char BackendErrorDisplayInUse[] = "Display in use!";
	constexpr const char BackendErrorRenderTargetSuboptimal[] = "Render target suboptimal!";
	constexpr const char BackendErrorRenderTrargetOutOfDate[] = "Render target out of date!";
	constexpr const char BackendErrorIncompatibleDisplay[] = "Incompatible display!";
	constexpr const char BackendErrorValidationFailed[] = "Validation failed!";
	constexpr const char BackendErrorInvalidShader[] = "Invalid shader!";
	constexpr const char BackendErrorOperationNotPermitted[] = "Operation not permitted!";
	constexpr const char BackendErrorFullScreenModeLost[] = "Full screen mode lost!";
	constexpr const char BackendErrorDeviceThreadIdle[] = "Device thread idle!";
	constexpr const char BackendErrorDeviceThreadDone[] = "Device thread done!";
	constexpr const char BackendErrorOperationDiferred[] = "Operation deferred!";
	constexpr const char BackendErrorOperationNotDeferred[] = "Operation not deferred!";
	constexpr const char BackendErrorPipelineCompilationRequired[] = "Pipeline compilation required!";
	constexpr const char BackendErrorVulkanLayerNotPresent[] = "Vulkan layer not present!";
	constexpr const char BackendErrorVulkanExtensionNotPresent[] = "Vulkan extension not present!";
	constexpr const char BackendErrorVulkanInvalidOpaqueCaptureAddress[] = "Vulkan invalid opaque capture address!";
}