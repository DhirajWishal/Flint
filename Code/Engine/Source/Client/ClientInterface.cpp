// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "Engine/Client/ClientInterface.hpp"

namespace Flint
{
	std::vector<std::string_view> ClientInterface::GetClassIdentifiers() const
	{
		return std::vector<std::string_view>(mRegisteredClasses.begin(), mRegisteredClasses.end());
	}

	void ClientInterface::RegisterIdentifier(const std::string& identifier)
	{
		mRegisteredClasses.push_back(identifier);
	}
	
	void ClientInterface::ActivateController(const std::string_view& identifier)
	{
		mActiveControllers[identifier] = CreateController(identifier);
	}

	void ClientInterface::DeactivateController(const std::string_view& identifier)
	{
		mActiveControllers.erase(identifier);
	}

	void ClientInterface::RunClient()
	{
	}
}