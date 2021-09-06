// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Engine/Client/ClientInterface.hpp"

namespace Flint
{
	/**
	 * Flint client loader object.
	 * This object acts as a wrapper to the client application upon loading.
	 */
	class ClientLoader
	{
	public:
		ClientLoader(const std::filesystem::path& executable);
		~ClientLoader();

		const ClientInterface* GetInterface() const { return pClientInterface; }

	private:
		void InitializeClient();

	private:
		ClientInterface* pClientInterface = nullptr;

		TCreateClientFunction mCreateClientFunction = nullptr;
		TDestroyClientFunction mDestroyClientFunction = nullptr;

		void* pModuleHandle = nullptr;
	};
}
