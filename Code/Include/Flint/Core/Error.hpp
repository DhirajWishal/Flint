// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "DataType.hpp"

#include <string>
#include <stdexcept>

namespace Flint
{
	/**
	 * Flint backend error exception.
	 * This error is thrown if there was a backend error.
	 * 
	 * This exception is a derived class of std::runtime_error
	 */
	class FLINT_API backend_error final : public std::runtime_error
	{
	public:
		backend_error(const std::string & msg) : std::runtime_error(msg) {}
		backend_error(const char* msg) : std::runtime_error(msg) {}
	};
}

#define FLINT_THROW_BACKEND_ERROR									throw ::Flint::backend_error
#define FLINT_THROW_LOGIC_ERROR										throw std::logic_error
#define FLINT_THROW_DOMAIN_ERROR									throw std::domain_error
#define FLINT_THROW_INVALID_ARGUMENT								throw std::invalid_argument
#define FLINT_THROW_LENGTH_ERROR									throw std::length_error
#define FLINT_THROW_OUT_OF_RANGE									throw std::out_of_range
#define FLINT_THROW_RUNTIME_ERROR									throw std::runtime_error
#define FLINT_THROW_RANGE_ERROR										throw std::range_error
#define FLINT_THROW_OVERFLOW_ERROR									throw std::overflow_error
#define FLINT_THROW_UNDERFLOW_ERROR									throw std::underflow_error

#ifdef FLINT_DEBUG
#	define FLINT_ASSERT(expression, condition, error)					if (expression != condition) error  
#	define FLINT_ASSERT_DISCARDABLE(expression, condition, error)		if (expression != condition) error;

#else
#	define FLINT_ASSERT_DISCARDABLE(expression, condition, error)	
#	define FLINT_ASSERT(expression, condition, error)					expression

#endif // FLINT_DEBUG

#define FLINT_BACKEND_ERROR_DEVICE_NOT_READY						"Device not ready!"
#define FLINT_BACKEND_ERROR_DEVICE_TIMEOUT							"Device timeout!"
#define FLINT_BACKEND_ERROR_EVENT_SET								"Event set!"
#define FLINT_BACKEND_ERROR_EVENT_RESET								"Event reset!"
#define FLINT_BACKEND_ERROR_INCOMPLETE_COMMAND						"Incomplete command!"
#define FLINT_BACKEND_ERROR_DEVICE_OUT_OF_HOST_MEMORY				"Device out of host memory!"
#define FLINT_BACKEND_ERROR_DEVICE_OUT_OF_DEVICE_MEMORY				"Device out of device memory!"
#define FLINT_BACKEND_ERROR_INITIALIZATION_FAILED					"Initialization failed!"
#define FLINT_BACKEND_ERROR_DEVICE_LOST								"Device lost!"
#define FLINT_BACKEND_ERROR_MEMORY_MAPPING_FAILED					"Memory mapping failed!"
#define FLINT_BACKEND_ERROR_VULKAN_LAYER_NOT_PRESENT				"Vulkan layer not present!"
#define FLINT_BACKEND_ERROR_VULKAN_EXTENSION_NOT_PRESENT			"Vulkan extension not present!"
#define FLINT_BACKEND_ERROR_DEVICE_FEATURE_NOT_PRESENT				"Device feature not present!"
#define FLINT_BACKEND_ERROR_INCOMPATIBLE_DRIVER						"Incompatible driver!"
#define FLINT_BACKEND_ERROR_TOO_MANY_OBJECTS						"Too many objects!"
#define FLINT_BACKEND_ERROR_FORMAT_NOT_SUPPORTED					"Format not supported!"
#define FLINT_BACKEND_ERROR_FRAGMENTED_POOL							"Fragmented pool!"
#define FLINT_BACKEND_ERROR_UNKNOWN									"Unknown error!"
#define FLINT_BACKEND_ERROR_OUT_OF_POOL_MEMORY						"Out of pool memory!"
#define FLINT_BACKEND_ERROR_INVALID_HANDLE							"Invalid handle!"
#define FLINT_BACKEND_ERROR_INTERNAL_FRAGMENTATION					"Internal fragmentation!"
#define FLINT_BACKEND_ERROR_VULKAN_INVALID_OPAQUE_CAPTURE_ADDRESS	"Vulkan invalid opaque capture address!"
#define FLINT_BACKEND_ERROR_DISPLAY_LOST							"Display lost!"
#define FLINT_BACKEND_ERROR_DISPLAY_IN_USE							"Display in use!"
#define FLINT_BACKEND_ERROR_SUBOPTIMAL_SWAP_CHAIN					"Suboptimal swap chain!"
#define FLINT_BACKEND_ERROR_RENDER_TARGET_OUT_OF_DATE				"Render target out of date!"
#define FLINT_BACKEND_ERROR_INCOMPATIBLE_DISPLAY					"Incompatible display!"
#define FLINT_BACKEND_ERROR_VALIDATION_FAILED						"Validation failed!"
#define FLINT_BACKEND_ERROR_INVALID_SHADER							"Invalid shader!"
#define FLINT_BACKEND_ERROR_OPERATION_NOT_PERMITTED					"Operation not permitted!"
#define FLINT_BACKEND_ERROR_FULL_SCREEN_MODE_LOST					"Full screen mode lost!"
#define FLINT_BACKEND_ERROR_DEVICE_THREAD_IDLE						"Device thread idle!"
#define FLINT_BACKEND_ERROR_DEVICE_THREAD_DONE						"Device thread done!"
#define FLINT_BACKEND_ERROR_OPERATION_DEFERRED						"Operation deferred!"
#define FLINT_BACKEND_ERROR_OPERATION_NOT_DEFERRED					"Operation not deferred!"
#define FLINT_BACKEND_ERROR_PIPELINE_COMPILATION_REQUIRED			"Pipeline compilation required!"
