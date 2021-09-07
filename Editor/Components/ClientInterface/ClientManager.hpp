// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#pragma once

#include "Engine/Client/ClientInterface.hpp"

namespace Flint
{
	/**
	 * Flint client manager object.
	 * This object acts as a wrapper to the client application upon loading.
	 */
	class ClientManager
	{
	public:
		ClientManager(const std::filesystem::path& executable);
		~ClientManager();

		void InitializeClient();
		void Update();

		ClientInterface* GetInterface() { return pClientInterface; }
		const ClientInterface* GetInterface() const { return pClientInterface; }

	private:
		std::filesystem::path mExecutablePath = {};
		std::filesystem::file_time_type mLastWriteTime = {};

		ClientInterface* pClientInterface = nullptr;

		TCreateClientFunction mCreateClientFunction = nullptr;
		TDestroyClientFunction mDestroyClientFunction = nullptr;

		void* pModuleHandle = nullptr;
	};
}
