// Copyright 2021 Dhiraj Wishal
// SPDX-License-Identifier: Apache-2.0

#include "ClientManager.hpp"

#ifdef FLINT_PLATFORM_WINDOWS

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#endif

namespace Flint
{
	ClientManager::ClientManager(const std::filesystem::path& executable)
	{
#ifdef FLINT_PLATFORM_WINDOWS
		if (executable.extension().string() != ".dll")
			throw std::invalid_argument("The provided file is not a valid windows executable!");

		mExecutablePath = executable;
		mLastWriteTime = std::filesystem::last_write_time(mExecutablePath);
		HMODULE pClientModule = LoadLibrary(mExecutablePath.c_str());

		if (!pClientModule)
			throw std::runtime_error("Failed to load the client executable!");

		mCreateClientFunction = reinterpret_cast<TCreateClientFunction>(GetProcAddress(pClientModule, "CreateClient"));
		mDestroyClientFunction = reinterpret_cast<TDestroyClientFunction>(GetProcAddress(pClientModule, "DestroyClient"));
		pModuleHandle = pClientModule;

#endif

		if (!mCreateClientFunction || !mDestroyClientFunction)
			throw std::runtime_error("Could not load the client symbols!");

		pClientInterface = mCreateClientFunction();
	}
	
	ClientManager::~ClientManager()
	{
#ifdef FLINT_PLATFORM_WINDOWS
		mDestroyClientFunction(pClientInterface);
		FreeLibrary(static_cast<HMODULE>(pModuleHandle));

#endif
	}

	void ClientManager::InitializeClient()
	{
		pClientInterface->ActivateController(pClientInterface->GetClassIdentifiers().front());
	}

	void ClientManager::Update()
	{
		auto lastWriteTime = std::filesystem::last_write_time(mExecutablePath);

		if (mLastWriteTime != lastWriteTime)
		{
			mLastWriteTime = lastWriteTime;

#ifdef FLINT_PLATFORM_WINDOWS
			mDestroyClientFunction(pClientInterface);
			FreeLibrary(static_cast<HMODULE>(pModuleHandle));

			HMODULE pClientModule = LoadLibrary(mExecutablePath.c_str());

			if (!pClientModule)
				throw std::runtime_error("Failed to load the client executable!");

			mCreateClientFunction = reinterpret_cast<TCreateClientFunction>(GetProcAddress(pClientModule, "CreateClient"));
			mDestroyClientFunction = reinterpret_cast<TDestroyClientFunction>(GetProcAddress(pClientModule, "DestroyClient"));
			pModuleHandle = pClientModule;
#endif

			if (!mCreateClientFunction || !mDestroyClientFunction)
				throw std::runtime_error("Could not load the client symbols!");

			pClientInterface = mCreateClientFunction();
			InitializeClient();
		}
	}
}