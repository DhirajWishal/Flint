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

#define FLINT_EXCEPTION_BACKEND_ERROR									::Flint::backend_error
#define FLINT_EXCEPTION_RUNTIME_ERROR									::Flint::runtime_error_extended
#define FLINT_EXCEPTION_LOGIC_ERROR										std::logic_error
#define FLINT_EXCEPTION_DOMAIN_ERROR									std::domain_error
#define FLINT_EXCEPTION_INVALID_ARGUMENT								std::invalid_argument
#define FLINT_EXCEPTION_LENGTH_ERROR									std::length_error
#define FLINT_EXCEPTION_OUT_OF_RANGE									std::out_of_range
#define FLINT_EXCEPTION_RANGE_ERROR										std::range_error
#define FLINT_EXCEPTION_OVERFLOW_ERROR									std::overflow_error
#define FLINT_EXCEPTION_UNDERFLOW_ERROR									std::underflow_error

#define FLINT_THROW_BACKEND_ERROR										throw FLINT_EXCEPTION_BACKEND_ERROR
#define FLINT_THROW_LOGIC_ERROR											throw FLINT_EXCEPTION_LOGIC_ERROR
#define FLINT_THROW_DOMAIN_ERROR										throw FLINT_EXCEPTION_DOMAIN_ERROR
#define FLINT_THROW_INVALID_ARGUMENT									throw FLINT_EXCEPTION_INVALID_ARGUMENT
#define FLINT_THROW_LENGTH_ERROR										throw FLINT_EXCEPTION_LENGTH_ERROR
#define FLINT_THROW_OUT_OF_RANGE										throw FLINT_EXCEPTION_OUT_OF_RANGE
#define FLINT_THROW_RANGE_ERROR											throw FLINT_EXCEPTION_RANGE_ERROR
#define FLINT_THROW_OVERFLOW_ERROR										throw FLINT_EXCEPTION_OVERFLOW_ERROR
#define FLINT_THROW_UNDERFLOW_ERROR										throw FLINT_EXCEPTION_UNDERFLOW_ERROR

#ifndef FLINT_RELEASE
#	define FLINT_THROW_RUNTIME_ERROR(...)								throw FLINT_EXCEPTION_RUNTIME_ERROR(__VA_ARGS__, "\nFile:\t\t" __FILE__ "\nFunction:\t" __FUNCSIG__ "\nLine:\t\t" TO_STRING(__LINE__))
#	define FLINT_ASSERT(expression, condition, error)					if (expression != condition) error
#	define FLINT_ASSERT_DISCARDABLE(expression, condition, error)		if (expression != condition) error;

#else
#	define FLINT_THROW_RUNTIME_ERROR									throw std::runtime_error
#	define FLINT_ASSERT(expression, condition, error)					expression
#	define FLINT_ASSERT_DISCARDABLE(expression, condition, error)

#endif // !FLINT_RELEASE

#define FLINT_BACKEND_ERROR_UNKNOWN										"Unknown error!"
#define FLINT_BACKEND_ERROR_DEVICE_NOT_READY							"Device not ready!"
#define FLINT_BACKEND_ERROR_DEVICE_TIMEOUT								"Device timeout!"
#define FLINT_BACKEND_ERROR_EVENT_SET									"Event set!"
#define FLINT_BACKEND_ERROR_EVENT_RESET									"Event reset!"
#define FLINT_BACKEND_ERROR_INCOMPLETE_COMMAND							"Incomplete command!"
#define FLINT_BACKEND_ERROR_DEVICE_OUT_OF_HOST_MEMORY					"Device out of host memory!"
#define FLINT_BACKEND_ERROR_DEVICE_OUT_OF_DEVICE_MEMORY					"Device out of device memory!"
#define FLINT_BACKEND_ERROR_INITIALIZATION_FAILED						"Initialization failed!"
#define FLINT_BACKEND_ERROR_DEVICE_LOST									"Device lost!"
#define FLINT_BACKEND_ERROR_MEMORY_MAPPING_FAILED						"Memory mapping failed!"
#define FLINT_BACKEND_ERROR_DEVICE_FEATURE_NOT_PRESENT					"Device feature not present!"
#define FLINT_BACKEND_ERROR_INCOMPATIBLE_DRIVER							"Incompatible driver!"
#define FLINT_BACKEND_ERROR_TOO_MANY_OBJECTS							"Too many objects!"
#define FLINT_BACKEND_ERROR_FORMAT_NOT_SUPPORTED						"Format not supported!"
#define FLINT_BACKEND_ERROR_FRAGMENTED_POOL								"Fragmented pool!"
#define FLINT_BACKEND_ERROR_OUT_OF_POOL_MEMORY							"Out of pool memory!"
#define FLINT_BACKEND_ERROR_INVALID_HANDLE								"Invalid handle!"
#define FLINT_BACKEND_ERROR_INTERNAL_FRAGMENTATION						"Internal fragmentation!"
#define FLINT_BACKEND_ERROR_DISPLAY_LOST								"Display lost!"
#define FLINT_BACKEND_ERROR_DISPLAY_IN_USE								"Display in use!"
#define FLINT_BACKEND_ERROR_RENDER_TARGET_SUBOPTIMAL					"Render target suboptimal!"
#define FLINT_BACKEND_ERROR_RENDER_TARGET_OUT_OF_DATE					"Render target out of date!"
#define FLINT_BACKEND_ERROR_INCOMPATIBLE_DISPLAY						"Incompatible display!"
#define FLINT_BACKEND_ERROR_VALIDATION_FAILED							"Validation failed!"
#define FLINT_BACKEND_ERROR_INVALID_SHADER								"Invalid shader!"
#define FLINT_BACKEND_ERROR_OPERATION_NOT_PERMITTED						"Operation not permitted!"
#define FLINT_BACKEND_ERROR_FULL_SCREEN_MODE_LOST						"Full screen mode lost!"
#define FLINT_BACKEND_ERROR_DEVICE_THREAD_IDLE							"Device thread idle!"
#define FLINT_BACKEND_ERROR_DEVICE_THREAD_DONE							"Device thread done!"
#define FLINT_BACKEND_ERROR_OPERATION_DEFERRED							"Operation deferred!"
#define FLINT_BACKEND_ERROR_OPERATION_NOT_DEFERRED						"Operation not deferred!"
#define FLINT_BACKEND_ERROR_PIPELINE_COMPILATION_REQUIRED				"Pipeline compilation required!"
#define FLINT_BACKEND_ERROR_VULKAN_LAYER_NOT_PRESENT					"Vulkan layer not present!"
#define FLINT_BACKEND_ERROR_VULKAN_EXTENSION_NOT_PRESENT				"Vulkan extension not present!"
#define FLINT_BACKEND_ERROR_VULKAN_INVALID_OPAQUE_CAPTURE_ADDRESS		"Vulkan invalid opaque capture address!"