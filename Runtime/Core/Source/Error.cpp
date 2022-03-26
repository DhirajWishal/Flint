// Copyright 2021-2022 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Core/Error.hpp"

namespace Flint
{
	backend_error::backend_error(const std::string& msg) : std::runtime_error(msg)
	{
	}

	backend_error::backend_error(const char* msg) : std::runtime_error(msg)
	{
	}
}