// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Error.hpp"

namespace Flint
{
	backend_error::backend_error(const std::string& msg) : std::runtime_error(msg)
	{
	}

	backend_error::backend_error(const char* msg) : std::runtime_error(msg)
	{
	}

	runtime_error_extended::runtime_error_extended(const std::string& msg, const char* pErrorLocation)
		: std::runtime_error(msg + pErrorLocation)
	{
	}

	runtime_error_extended::runtime_error_extended(const char* msg, const char* pErrorLocation)
		: std::runtime_error(std::string(msg) + pErrorLocation)
	{
	}
}