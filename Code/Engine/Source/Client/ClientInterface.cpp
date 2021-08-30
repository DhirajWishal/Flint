// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Engine/Client/ClientInterface.hpp"

namespace Flint
{
	std::vector<std::string_view> ClientInterface::GetClassIdentifiers() const
	{
		return std::vector<std::string_view>(mRegisteredClasses.begin(), mRegisteredClasses.end());
	}
}